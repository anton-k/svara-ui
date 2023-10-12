.PHONY: build

build:
	cmake --build cmake-build

run: build
	./cmake-build/GuiAppExample_artefacts/Gui\ App\ Example \
			--csound=examples/gain.csd --ui=examples/gain-csound.yaml

config:
	cmake -B cmake-build-install -DCMAKE_INSTALL_PREFIX=/home/anton/dev/cpp/JUCE/install

# HOWTO: create JUCE project
#
# https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md
#
# How to package with CPack
# * https://linuxhint.com/cmake-cpack-linux/
# * https://cmake.org/cmake/help/book/mastering-cmake/chapter/Packaging%20With%20CPack.html
# * https://embeddeduse.com/2020/03/21/creating-simple-installers-with-cpack/
