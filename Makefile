
.PHONY: clean

check:
ifndef VULKAN_SDK
	$(error VULKAN_SDK must be set)
endif

build/setup.stamp: check projectConfig.json
	./vendor/premake/premake5 gmake2
	touch $@

setup: build/setup.stamp

compile-debug: setup
	make -C build/gmake2/ -j4

compile-release: setup
	make -C build/gmake2/ -j4 config=release

run-debug:
	cd data/ && ../build/gmake2/bin-debug/sandbox

run-release:
	cd data/ && ../build/gmake2/bin-release/sandbox

clean:
	make -C build/gmake2/ clean

purge:
	rm -rf build/