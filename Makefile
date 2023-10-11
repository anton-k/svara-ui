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
