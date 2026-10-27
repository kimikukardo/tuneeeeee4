#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <vector>
#include "PitchDetector.h"
#include "PitchShifter.h"
#include "NoteUtils.h"

class RealtuneAudioProcessor : public juce::AudioProcessor
{
public:
    RealtuneAudioProcessor();
    ~RealtuneAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Realtune"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    juce::AudioProcessorValueTreeState apvts;

    // Para que el editor muestre la nota detectada en vivo
    std::atomic<float> detectedFrequencyHz { 0.0f };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    PitchDetector pitchDetector;
    PitchShifter pitchShifter;

    static constexpr int analysisWindowSize = 1024;
    std::vector<float> analysisWindow;
    int analysisWritePos = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtuneAudioProcessor)
};
