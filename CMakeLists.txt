# Example GUI App CMakeLists.txt

# To get started on a new GUI app, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications. For other examples of CMakeLists for
# GUI apps, check `extras/Projucer` and `examples/DemoRunner` in the JUCE repo.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.15)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(GUI_APP_EXAMPLE VERSION 0.0.1)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.


# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
add_subdirectory(libs/juce)                    # If you've put JUCE in a subdirectory called JUCE

# If your app depends the VST2 SDK, perhaps to host VST2 plugins, CMake needs to be told where
# to find the SDK on your system. This setup should be done before calling `juce_add_gui_app`.

# juce_set_vst2_sdk_path(...)

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (GuiAppExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

add_subdirectory("libs/yaml-cpp")

# How to include HEADER only libs so that hey do not produce warnings
add_library(plog INTERFACE)
target_sources(plog INTERFACE "libs/plog/include")
target_include_directories(plog SYSTEM INTERFACE "libs/plog/include")

# include_directories(libs/fontaudio/wrappers/juce)
# target_link_libraries(fontaudio libs/fontaudio/wrappers/juce/fontaudio.a )
# target_include_directories(fontaudio PUBLIC libs/fontaudio/wrappers/juce/)

juce_add_gui_app(GuiAppExample
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "Gui App Example")     # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(GuiAppExample)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(GuiAppExample
    PRIVATE
        src/app/desktop/Main.cpp
        src/app/desktop/MockUi.cpp
        src/audio/CsoundApp.cpp
        src/audio/CsoundProcessor.cpp
        src/ui/model/Model.cpp
        src/ui/App.cpp
        src/ui/Palette.cpp
        src/ui/Build.cpp
        src/ui/parser/Yaml.cpp
        src/ui/parser/Parser.cpp
        src/ui/parser/Expr.cpp
        src/ui/widgets/Dot.cpp
        src/ui/widgets/Meter.cpp
        src/ui/widgets/XYPad.cpp
        src/ui/widgets/ToggleGroup.cpp
        src/ui/widgets/KeyPressListener.cpp
        src/ui/Style.cpp
        )

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(GuiAppExample
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:GuiAppExample,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:GuiAppExample,JUCE_VERSION>"
        JUCE_DISPLAY_SPLASH_SCREEN=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# example
# https://git.iem.at/audioplugins/IEMPluginSuite/-/blob/master/SimpleDecoder/CMakeLists.txt
# juce_add_binary_data(GuiAppData SOURCES ...)
juce_add_binary_data(FontaudioIcons
  HEADER_NAME "Icons.h"
  NAMESPACE Icons
  SOURCES
  libs/fontaudio/svgs/fad-ADR.svg
  libs/fontaudio/svgs/fad-ADSR.svg
  libs/fontaudio/svgs/fad-AHDSR.svg
  libs/fontaudio/svgs/fad-armrecording.svg
  libs/fontaudio/svgs/fad-arpchord.svg
  libs/fontaudio/svgs/fad-arpdownandup.svg
  libs/fontaudio/svgs/fad-arpdown.svg
  libs/fontaudio/svgs/fad-arpdownup.svg
  libs/fontaudio/svgs/fad-arpplayorder.svg
  libs/fontaudio/svgs/fad-arprandom.svg
  libs/fontaudio/svgs/fad-arpupandown.svg
  libs/fontaudio/svgs/fad-arpupdown.svg
  libs/fontaudio/svgs/fad-arpup.svg
  libs/fontaudio/svgs/fad-arrows-horz.svg
  libs/fontaudio/svgs/fad-arrows-vert.svg
  libs/fontaudio/svgs/fad-AR.svg
  libs/fontaudio/svgs/fad-automation-2p.svg
  libs/fontaudio/svgs/fad-automation-3p.svg
  libs/fontaudio/svgs/fad-automation-4p.svg
  libs/fontaudio/svgs/fad-backward.svg
  libs/fontaudio/svgs/fad-bluetooth.svg
  libs/fontaudio/svgs/fad-caret-down.svg
  libs/fontaudio/svgs/fad-caret-left.svg
  libs/fontaudio/svgs/fad-caret-right.svg
  libs/fontaudio/svgs/fad-caret-up.svg
  libs/fontaudio/svgs/fad-close.svg
  libs/fontaudio/svgs/fad-copy.svg
  libs/fontaudio/svgs/fad-cpu.svg
  libs/fontaudio/svgs/fad-cutter.svg
  libs/fontaudio/svgs/fad-digital0.svg
  libs/fontaudio/svgs/fad-digital1.svg
  libs/fontaudio/svgs/fad-digital2.svg
  libs/fontaudio/svgs/fad-digital3.svg
  libs/fontaudio/svgs/fad-digital4.svg
  libs/fontaudio/svgs/fad-digital5.svg
  libs/fontaudio/svgs/fad-digital6.svg
  libs/fontaudio/svgs/fad-digital7.svg
  libs/fontaudio/svgs/fad-digital8.svg
  libs/fontaudio/svgs/fad-digital9.svg
  libs/fontaudio/svgs/fad-digital-colon.svg
  libs/fontaudio/svgs/fad-digital-dot.svg
  libs/fontaudio/svgs/fad-diskio.svg
  libs/fontaudio/svgs/fad-drumpad.svg
  libs/fontaudio/svgs/fad-duplicate.svg
  libs/fontaudio/svgs/fad-eraser.svg
  libs/fontaudio/svgs/fad-ffwd.svg
  libs/fontaudio/svgs/fad-filter-bandpass.svg
  libs/fontaudio/svgs/fad-filter-bell.svg
  libs/fontaudio/svgs/fad-filter-bypass.svg
  libs/fontaudio/svgs/fad-filter-highpass.svg
  libs/fontaudio/svgs/fad-filter-lowpass.svg
  libs/fontaudio/svgs/fad-filter-notch.svg
  libs/fontaudio/svgs/fad-filter-rez-highpass.svg
  libs/fontaudio/svgs/fad-filter-rez-lowpass.svg
  libs/fontaudio/svgs/fad-filter-shelving-hi.svg
  libs/fontaudio/svgs/fad-filter-shelving-lo.svg
  libs/fontaudio/svgs/fad-foldback.svg
  libs/fontaudio/svgs/fad-forward.svg
  libs/fontaudio/svgs/fad-hardclipcurve.svg
  libs/fontaudio/svgs/fad-hardclip.svg
  libs/fontaudio/svgs/fad-headphones.svg
  libs/fontaudio/svgs/fad-h-expand.svg
  libs/fontaudio/svgs/fad-keyboard.svg
  libs/fontaudio/svgs/fad-lock.svg
  libs/fontaudio/svgs/fad-logo-aax.svg
  libs/fontaudio/svgs/fad-logo-abletonlink.svg
  libs/fontaudio/svgs/fad-logo-audacity.svg
  libs/fontaudio/svgs/fad-logo-audiobus.svg
  libs/fontaudio/svgs/fad-logo-au.svg
  libs/fontaudio/svgs/fad-logo-cubase.svg
  libs/fontaudio/svgs/fad-logo-fl.svg
  libs/fontaudio/svgs/fad-logo-juce.svg
  libs/fontaudio/svgs/fad-logo-ladspa.svg
  libs/fontaudio/svgs/fad-logo-live.svg
  libs/fontaudio/svgs/fad-logo-lv2.svg
  libs/fontaudio/svgs/fad-logo-protools.svg
  libs/fontaudio/svgs/fad-logo-rackext.svg
  libs/fontaudio/svgs/fad-logo-reaper.svg
  libs/fontaudio/svgs/fad-logo-reason.svg
  libs/fontaudio/svgs/fad-logo-rewire.svg
  libs/fontaudio/svgs/fad-logo-studioone.svg
  libs/fontaudio/svgs/fad-logo-tracktion.svg
  libs/fontaudio/svgs/fad-logo-vst.svg
  libs/fontaudio/svgs/fad-logo-waveform.svg
  libs/fontaudio/svgs/fad-loop.svg
  libs/fontaudio/svgs/fad-metronome.svg
  libs/fontaudio/svgs/fad-microphone.svg
  libs/fontaudio/svgs/fad-midiplug.svg
  libs/fontaudio/svgs/fad-modrandom.svg
  libs/fontaudio/svgs/fad-modsawdown.svg
  libs/fontaudio/svgs/fad-modsawup.svg
  libs/fontaudio/svgs/fad-modsh.svg
  libs/fontaudio/svgs/fad-modsine.svg
  libs/fontaudio/svgs/fad-modsquare.svg
  libs/fontaudio/svgs/fad-modtri.svg
  libs/fontaudio/svgs/fad-modularplug.svg
  libs/fontaudio/svgs/fad-mono.svg
  libs/fontaudio/svgs/fad-mute.svg
  libs/fontaudio/svgs/fad-next.svg
  libs/fontaudio/svgs/fad-open.svg
  libs/fontaudio/svgs/fad-paste.svg
  libs/fontaudio/svgs/fad-pause.svg
  libs/fontaudio/svgs/fad-pen.svg
  libs/fontaudio/svgs/fad-phase.svg
  libs/fontaudio/svgs/fad-play.svg
  libs/fontaudio/svgs/fad-pointer.svg
  libs/fontaudio/svgs/fad-powerswitch.svg
  libs/fontaudio/svgs/fad-preset-ab.svg
  libs/fontaudio/svgs/fad-preset-a.svg
  libs/fontaudio/svgs/fad-preset-ba.svg
  libs/fontaudio/svgs/fad-preset-b.svg
  libs/fontaudio/svgs/fad-prev.svg
  libs/fontaudio/svgs/fad-punch-in.svg
  libs/fontaudio/svgs/fad-punch-out.svg
  libs/fontaudio/svgs/fad-ram.svg
  libs/fontaudio/svgs/fad-random-1dice.svg
  libs/fontaudio/svgs/fad-random-2dice.svg
  libs/fontaudio/svgs/fad-record.svg
  libs/fontaudio/svgs/fad-redo.svg
  libs/fontaudio/svgs/fad-repeat-one.svg
  libs/fontaudio/svgs/fad-repeat.svg
  libs/fontaudio/svgs/fad-rew.svg
  libs/fontaudio/svgs/fad-roundswitch-off.svg
  libs/fontaudio/svgs/fad-roundswitch-on.svg
  libs/fontaudio/svgs/fad-saveas.svg
  libs/fontaudio/svgs/fad-save.svg
  libs/fontaudio/svgs/fad-scissors.svg
  libs/fontaudio/svgs/fad-shuffle.svg
  libs/fontaudio/svgs/fad-sliderhandle-1.svg
  libs/fontaudio/svgs/fad-sliderhandle-2.svg
  libs/fontaudio/svgs/fad-slider-round-1.svg
  libs/fontaudio/svgs/fad-slider-round-2.svg
  libs/fontaudio/svgs/fad-slider-round-3.svg
  libs/fontaudio/svgs/fad-softclipcurve.svg
  libs/fontaudio/svgs/fad-softclip.svg
  libs/fontaudio/svgs/fad-solo.svg
  libs/fontaudio/svgs/fad-speaker.svg
  libs/fontaudio/svgs/fad-squareswitch-off.svg
  libs/fontaudio/svgs/fad-squareswitch-on.svg
  libs/fontaudio/svgs/fad-stereo.svg
  libs/fontaudio/svgs/fad-stop.svg
  libs/fontaudio/svgs/fad-thunderbolt.svg
  libs/fontaudio/svgs/fad-timeselect.svg
  libs/fontaudio/svgs/fad-undo.svg
  libs/fontaudio/svgs/fad-unlock.svg
  libs/fontaudio/svgs/fad-usb.svg
  libs/fontaudio/svgs/fad-v-expand.svg
  libs/fontaudio/svgs/fad-vroundswitch-off.svg
  libs/fontaudio/svgs/fad-vroundswitch-on.svg
  libs/fontaudio/svgs/fad-vsquareswitch-off.svg
  libs/fontaudio/svgs/fad-vsquareswitch-on.svg
  libs/fontaudio/svgs/fad-waveform.svg
  libs/fontaudio/svgs/fad-xlrplug.svg
  libs/fontaudio/svgs/fad-zoomin.svg
  libs/fontaudio/svgs/fad-zoomout.svg  
)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_gui_extra` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

include_directories("/usr/include/csound")

# see JUCE/examples/DemoRunner/CmakeLists.txt for examples of extra flags and utils
target_link_libraries(GuiAppExample
    PRIVATE
        # GuiAppData            # If we'd created a binary data target, we'd link to it here
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_audio_devices
        FontaudioIcons
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        plog
        yaml-cpp
        csound64
        )

      include_directories("../MyStaticLibrary")

      target_compile_options(GuiAppExample PRIVATE -Wall -Wextra)

# How to load images on linux
# https://github.com/monocasual/giada/blob/master/CMakeLists.txt

