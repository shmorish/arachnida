#include "scorpion.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <map>
#include <cstring>

std::vector<std::string> getSupportedExtensions() {
    return {".jpg", ".jpeg", ".png", ".gif", ".bmp"};
}

bool isValidImageFile(const std::string& filePath) {
    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0) {
        return false;
    }

    if (!S_ISREG(buffer.st_mode)) {
        return false;
    }

    std::string lowerPath = filePath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

    auto extensions = getSupportedExtensions();
    for (const auto& ext : extensions) {
        if (lowerPath.length() >= ext.length() &&
            lowerPath.compare(lowerPath.length() - ext.length(), ext.length(), ext) == 0) {
            return true;
        }
    }

    return false;
}

void displayBasicFileInfo(const std::string& filePath) {
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        std::cerr << "Cannot get file statistics\n";
        return;
    }

    std::cout << "File: " << filePath << "\n";
    std::cout << "Size: " << fileStat.st_size << " bytes\n";

    char timeBuffer[100];
    struct tm* timeinfo;

    #ifdef __APPLE__
    timeinfo = localtime(&fileStat.st_birthtimespec.tv_sec);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Created: " << timeBuffer << "\n";
    #endif

    timeinfo = localtime(&fileStat.st_mtime);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Modified: " << timeBuffer << "\n";

    timeinfo = localtime(&fileStat.st_atime);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Accessed: " << timeBuffer << "\n";
}

// Helper functions for reading multi-byte values
uint16_t readUint16(const std::vector<uint8_t>& data, size_t offset, bool bigEndian) {
    if (offset + 1 >= data.size()) return 0;
    if (bigEndian) {
        return (data[offset] << 8) | data[offset + 1];
    } else {
        return data[offset] | (data[offset + 1] << 8);
    }
}

uint32_t readUint32(const std::vector<uint8_t>& data, size_t offset, bool bigEndian) {
    if (offset + 3 >= data.size()) return 0;
    if (bigEndian) {
        return (data[offset] << 24) | (data[offset + 1] << 16) |
               (data[offset + 2] << 8) | data[offset + 3];
    } else {
        return data[offset] | (data[offset + 1] << 8) |
               (data[offset + 2] << 16) | (data[offset + 3] << 24);
    }
}

std::string getExifTagName(uint16_t tag) {
    static const std::map<uint16_t, std::string> tagNames = {
        {0x010E, "Image Description"},
        {0x010F, "Make"},
        {0x0110, "Model"},
        {0x0112, "Orientation"},
        {0x011A, "X Resolution"},
        {0x011B, "Y Resolution"},
        {0x0128, "Resolution Unit"},
        {0x0131, "Software"},
        {0x0132, "DateTime"},
        {0x013B, "Artist"},
        {0x8298, "Copyright"},
        {0x829A, "Exposure Time"},
        {0x829D, "F Number"},
        {0x8822, "Exposure Program"},
        {0x8827, "ISO Speed Ratings"},
        {0x9000, "EXIF Version"},
        {0x9003, "Date Time Original"},
        {0x9004, "Date Time Digitized"},
        {0x9201, "Shutter Speed Value"},
        {0x9202, "Aperture Value"},
        {0x9203, "Brightness Value"},
        {0x9204, "Exposure Bias Value"},
        {0x9205, "Max Aperture Value"},
        {0x9207, "Metering Mode"},
        {0x9208, "Light Source"},
        {0x9209, "Flash"},
        {0x920A, "Focal Length"},
        {0x927C, "Maker Note"},
        {0x9286, "User Comment"},
        {0xA000, "FlashPix Version"},
        {0xA001, "Color Space"},
        {0xA002, "Pixel X Dimension"},
        {0xA003, "Pixel Y Dimension"},
        {0xA20E, "Focal Plane X Resolution"},
        {0xA20F, "Focal Plane Y Resolution"},
        {0xA210, "Focal Plane Resolution Unit"},
        {0xA217, "Sensing Method"},
        {0xA300, "File Source"},
        {0xA301, "Scene Type"},
        {0xA402, "Exposure Mode"},
        {0xA403, "White Balance"},
        {0xA404, "Digital Zoom Ratio"},
        {0xA405, "Focal Length In 35mm Film"},
        {0xA406, "Scene Capture Type"},
        {0xA430, "Camera Owner Name"},
        {0xA431, "Body Serial Number"},
        {0xA432, "Lens Specification"},
        {0xA433, "Lens Make"},
        {0xA434, "Lens Model"},
        {0xA435, "Lens Serial Number"},
    };

    auto it = tagNames.find(tag);
    if (it != tagNames.end()) {
        return it->second;
    }
    return "Unknown Tag 0x" + std::to_string(tag);
}

std::string formatExifValue(const std::vector<uint8_t>& data, size_t valueOffset,
                           uint16_t type, uint32_t count, bool bigEndian) {
    std::string result;

    switch (type) {
        case 1: // BYTE
            if (count == 1 && valueOffset < data.size()) {
                result = std::to_string(data[valueOffset]);
            }
            break;
        case 2: // ASCII
            if (valueOffset + count <= data.size()) {
                result = std::string(data.begin() + valueOffset,
                                   data.begin() + valueOffset + count - 1);
            }
            break;
        case 3: // SHORT
            if (count == 1) {
                result = std::to_string(readUint16(data, valueOffset, bigEndian));
            }
            break;
        case 4: // LONG
            if (count == 1) {
                result = std::to_string(readUint32(data, valueOffset, bigEndian));
            }
            break;
        case 5: // RATIONAL
            if (count == 1 && valueOffset + 7 < data.size()) {
                uint32_t numerator = readUint32(data, valueOffset, bigEndian);
                uint32_t denominator = readUint32(data, valueOffset + 4, bigEndian);
                if (denominator != 0) {
                    result = std::to_string(numerator) + "/" + std::to_string(denominator);
                    double value = static_cast<double>(numerator) / denominator;
                    result += " (" + std::to_string(value) + ")";
                }
            }
            break;
    }

    return result;
}

void parseExifIFD(const std::vector<uint8_t>& data, size_t ifdOffset,
                  size_t tiffStart, bool bigEndian) {
    if (ifdOffset + tiffStart + 1 >= data.size()) return;

    uint16_t numEntries = readUint16(data, tiffStart + ifdOffset, bigEndian);

    std::cout << "\nEXIF Data:\n";
    std::cout << std::string(80, '=') << "\n";

    size_t entryOffset = tiffStart + ifdOffset + 2;

    for (uint16_t i = 0; i < numEntries && entryOffset + 11 < data.size(); ++i) {
        uint16_t tag = readUint16(data, entryOffset, bigEndian);
        uint16_t type = readUint16(data, entryOffset + 2, bigEndian);
        uint32_t count = readUint32(data, entryOffset + 4, bigEndian);
        uint32_t valueOffset = readUint32(data, entryOffset + 8, bigEndian);

        std::string tagName = getExifTagName(tag);

        // Value size calculation
        static const uint8_t typeSizes[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};
        uint32_t valueSize = 0;
        if (type < sizeof(typeSizes)) {
            valueSize = typeSizes[type] * count;
        }

        size_t actualValueOffset;
        if (valueSize <= 4) {
            actualValueOffset = entryOffset + 8;
        } else {
            actualValueOffset = tiffStart + valueOffset;
        }

        std::string value = formatExifValue(data, actualValueOffset, type, count, bigEndian);

        if (!value.empty()) {
            std::cout << std::left << std::setw(30) << tagName << ": " << value << "\n";
        }

        entryOffset += 12;
    }
}

void parseJpegExif(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file\n";
        return;
    }

    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();

    if (buffer.size() < 2 || buffer[0] != 0xFF || buffer[1] != 0xD8) {
        return;
    }

    size_t pos = 2;
    bool foundExif = false;

    while (pos + 3 < buffer.size()) {
        if (buffer[pos] != 0xFF) break;

        uint8_t marker = buffer[pos + 1];
        uint16_t length = (buffer[pos + 2] << 8) | buffer[pos + 3];

        if (marker == 0xE1) {
            if (pos + 10 < buffer.size() &&
                buffer[pos + 4] == 'E' && buffer[pos + 5] == 'x' &&
                buffer[pos + 6] == 'i' && buffer[pos + 7] == 'f' &&
                buffer[pos + 8] == 0 && buffer[pos + 9] == 0) {

                foundExif = true;
                size_t tiffStart = pos + 10;

                if (tiffStart + 7 >= buffer.size()) break;

                bool bigEndian = (buffer[tiffStart] == 'M' && buffer[tiffStart + 1] == 'M');

                uint32_t ifdOffset = readUint32(buffer, tiffStart + 4, bigEndian);

                parseExifIFD(buffer, ifdOffset, tiffStart, bigEndian);
                break;
            }
        }

        pos += 2 + length;
    }

    if (!foundExif) {
        std::cout << "\nNo EXIF data found\n";
    }
}

void parsePngMetadata(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file\n";
        return;
    }

    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();

    const uint8_t pngSignature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (buffer.size() < 8 || memcmp(buffer.data(), pngSignature, 8) != 0) {
        return;
    }

    std::cout << "\nPNG Metadata:\n";
    std::cout << std::string(80, '=') << "\n";

    size_t pos = 8;
    bool foundMetadata = false;

    while (pos + 12 <= buffer.size()) {
        uint32_t length = (buffer[pos] << 24) | (buffer[pos + 1] << 16) |
                         (buffer[pos + 2] << 8) | buffer[pos + 3];

        std::string chunkType(buffer.begin() + pos + 4, buffer.begin() + pos + 8);

        if (chunkType == "tEXt" || chunkType == "zTXt" || chunkType == "iTXt") {
            foundMetadata = true;
            if (pos + 8 + length <= buffer.size()) {
                std::string text(buffer.begin() + pos + 8, buffer.begin() + pos + 8 + length);
                size_t nullPos = text.find('\0');
                if (nullPos != std::string::npos) {
                    std::string keyword = text.substr(0, nullPos);
                    std::string value = text.substr(nullPos + 1);
                    std::cout << std::left << std::setw(30) << keyword << ": " << value << "\n";
                }
            }
        }

        if (chunkType == "IEND") break;

        pos += 12 + length;
    }

    if (!foundMetadata) {
        std::cout << "No text metadata found\n";
    }
}

void displayImageMetadata(const std::string& filePath) {
    displayBasicFileInfo(filePath);

    std::string lowerPath = filePath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

    if (lowerPath.find(".jpg") != std::string::npos ||
        lowerPath.find(".jpeg") != std::string::npos) {
        parseJpegExif(filePath);
    } else if (lowerPath.find(".png") != std::string::npos) {
        parsePngMetadata(filePath);
    } else {
        std::cout << "\nMetadata parsing not yet implemented for this format\n";
        std::cout << "Basic file information only\n";
    }
}
