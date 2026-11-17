cmake_minimum_required(VERSION 3.22)

project(VstChorus VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE automatically (no manual SDK setup needed)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.15
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(VstChorus
    COMPANY_NAME "Violent Studios"
    BUNDLE_ID com.violentstudios.vstchorus
    PLUGIN_MANUFACTURER_CODE Vlnt
    PLUGIN_CODE Vchr
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "VST Violent Chorus"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(VstChorus
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
)

juce_add_binary_data(VstChorusAssets
    SOURCES
        assets/logo.png
)

target_compile_definitions(VstChorus
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VstChorus
    PRIVATE
        VstChorusAssets
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
