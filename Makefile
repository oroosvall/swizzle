
.PHONY: clean compile-commands

check:
ifndef VULKAN_SDK
	$(error VULKAN_SDK must be set)
endif

build/setup.stamp: check projectConfig.json
	./vendor/premake/premake5 gmake2
	touch $@

setup: build/setup.stamp

compile-commands:
	cd build/gmake2/ && make clean
	cd build/gmake2/ && bear intercept -- make -j
	cd build/gmake2/ && bear citnames 
	cd build/gmake2/ && make clean
	cp build/gmake2/compile_commands.json .vscode/

compile-debug: setup
	make -C build/gmake2/ -j

compile-release: setup
	make -C build/gmake2/ -j config=release

run-debug: compile-debug
	cd data/ && ../build/gmake2/bin-debug/sandbox

run-release: compile-release
	cd data/ && ../build/gmake2/bin-release/sandbox

clean:
	make -C build/gmake2/ clean
	make -C build/gmake2/ clean config=release

purge:
	rm -rf build/
