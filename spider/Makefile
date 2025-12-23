.PHONY: all build clean fclean re run

BUILD_DIR = build

all: build

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && cmake --build .

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -f spider

re: fclean all

run: build
	@./spider
