.PHONY: all build clean fclean re run

BUILD_DIR = build

all: build

update_submodules:
	@git submodule update --init --recursive

build:
	@if [ ! -f vendor/curl/CMakeLists.txt ]; then \
		echo "Initializing submodules..."; \
		$(MAKE) update_submodules; \
	fi
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && cmake --build .

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -f spider
	@rm -rf vendor

re: fclean all

run: build
	@./spider
