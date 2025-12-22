.PHONY: all build clean re run

BUILD_DIR = build

all: build

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && cmake --build .

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f spider

re: clean all

run: build
	@./spider
