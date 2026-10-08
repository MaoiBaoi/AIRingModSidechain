cmake_minimum_required(VERSION 3.22)

# Windows: Laufzeitbibliothek fest einbinden, damit auf deinem Rechner
# keine zusaetzliche "Visual C++ Redistributable" installiert sein muss.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(RingModSidechain VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.8
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(RingModSidechain
    COMPANY_NAME             "MyCompany"
    PLUGIN_MANUFACTURER_CODE Mycp
    PLUGIN_CODE              Rmsc
    FORMATS                  VST3
    PRODUCT_NAME             "RingModSidechain"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT           FALSE
    VST3_CATEGORIES          Fx Modulation
    COPY_PLUGIN_AFTER_BUILD  FALSE
)

target_sources(RingModSidechain PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(RingModSidechain PRIVATE cxx_std_17)

target_compile_definitions(RingModSidechain PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RingModSidechain
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
