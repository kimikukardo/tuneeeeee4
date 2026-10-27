#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeelOmni.h"
#include "CentralDisplay.h"

class RealtuneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit RealtuneAudioProcessorEditor (RealtuneAudioProcessor&);
    ~RealtuneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Mini teclado que resalta las notas de la escala/tonica elegidas (como en Waves Tune RT)
    class ScaleKeyboard : public juce::Component
    {
    public:
        std::function<int()> getKeyRoot;
        std::function<std::vector<int>()> getScaleSemitones;
        void paint (juce::Graphics& g) override;
    };

    RealtuneAudioProcessor& processor;
    LookAndFeelOmni omniLookAndFeel;

    CentralDisplay centralDisplay;
    ScaleKeyboard keyboard;

    juce::ComboBox keyBox, scaleBox;
    juce::Slider speedKnob, mixKnob;
    juce::Label speedLabel, mixLabel, titleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttachment, scaleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment, mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtuneAudioProcessorEditor)
};
