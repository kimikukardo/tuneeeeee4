cmake_minimum_required(VERSION 3.22)
project(Realtune VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Esta linea asume que la carpeta "JUCE" (descargada de juce.com) esta
# como carpeta HERMANA de este proyecto, es decir:
#   TuCarpeta/
#     JUCE/          <- SDK de JUCE descargado
#     Realtune/      <- este proyecto
# Si la pusiste en otro lado, cambia la ruta de abajo.
add_subdirectory(../JUCE JUCE)

juce_add_plugin(Realtune
    COMPANY_NAME "TuNombre"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Rltn
    PLUGIN_CODE Rlt1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Realtune")
# Nota: AU (Audio Unit) solo se puede compilar en macOS con Xcode.
# Si mas adelante usas una Mac, agrega "AU" a FORMATS arriba.

juce_generate_juce_header(Realtune)

target_sources(Realtune
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/PitchDetector.cpp
        Source/PitchShifter.cpp
        Source/LookAndFeelOmni.cpp
        Source/CentralDisplay.cpp)

target_compile_definitions(Realtune
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Realtune
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
