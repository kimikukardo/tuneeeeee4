#include "PluginEditor.h"
#include "NoteUtils.h"
#include <algorithm>

void RealtuneAudioProcessorEditor::ScaleKeyboard::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (LookAndFeelOmni::chassisBlack);
    g.fillRoundedRectangle (bounds, 4.0f);

    int root = getKeyRoot ? getKeyRoot() : 0;
    auto semis = getScaleSemitones ? getScaleSemitones() : std::vector<int> { 0,2,4,5,7,9,11 };

    const int numKeys = 12;
    float keyWidth = bounds.getWidth() / (float) numKeys;

    for (int i = 0; i < numKeys; ++i)
    {
        auto keyRect = bounds.removeFromLeft (keyWidth).reduced (1.0f);
        int semitoneFromRoot = ((i - root) % 12 + 12) % 12;
        bool inScale = std::find (semis.begin(), semis.end(), semitoneFromRoot) != semis.end();

        g.setColour (inScale ? LookAndFeelOmni::omniGreen : LookAndFeelOmni::spikeGrey.withAlpha (0.5f));
        g.fillRoundedRectangle (keyRect, 2.0f);
    }
}

RealtuneAudioProcessorEditor::RealtuneAudioProcessorEditor (RealtuneAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      centralDisplay ([&p] { return p.detectedFrequencyHz.load(); })
{
    setLookAndFeel (&omniLookAndFeel);

    addAndMakeVisible (centralDisplay);
    addAndMakeVisible (keyboard);

    keyboard.getKeyRoot = [&p] { return (int) p.apvts.getRawParameterValue ("key")->load(); };
    keyboard.getScaleSemitones = [&p]
    {
        int scaleIdx = (int) p.apvts.getRawParameterValue ("scale")->load();
        return NoteUtils::scaleSemitones ((NoteUtils::ScaleType) scaleIdx);
    };

    juce::StringArray keyNames { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    keyBox.addItemList (keyNames, 1);
    addAndMakeVisible (keyBox);
    keyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.apvts, "key", keyBox);

    juce::StringArray scaleNames { "Chromatic","Major","Natural Minor","Harmonic Minor","Major Pentatonic","Minor Pentatonic" };
    scaleBox.addItemList (scaleNames, 1);
    addAndMakeVisible (scaleBox);
    scaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.apvts, "scale", scaleBox);

    for (auto* knob : { &speedKnob, &mixKnob })
    {
        knob->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knob->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
        addAndMakeVisible (knob);
    }

    speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, "speed", speedKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, "mix", mixKnob);

    for (auto* label : { &speedLabel, &mixLabel })
    {
        label->setJustificationType (juce::Justification::centred);
        label->setColour (juce::Label::textColourId, LookAndFeelOmni::omniGreen);
        addAndMakeVisible (label);
    }
    speedLabel.setText ("SPEED", juce::dontSendNotification);
    mixLabel.setText ("MIX", juce::dontSendNotification);

    titleLabel.setText ("REALTUNE", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, LookAndFeelOmni::capWhite);
    titleLabel.setFont (juce::Font (22.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    setSize (480, 420);
}

RealtuneAudioProcessorEditor::~RealtuneAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void RealtuneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (LookAndFeelOmni::chassisBlack);
}

void RealtuneAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);

    titleLabel.setBounds (area.removeFromTop (28));

    auto keyboardArea = area.removeFromBottom (28);
    keyboard.setBounds (keyboardArea);

    auto selectorsArea = area.removeFromBottom (36);
    keyBox.setBounds (selectorsArea.removeFromLeft (selectorsArea.getWidth() / 2).reduced (4, 0));
    scaleBox.setBounds (selectorsArea.reduced (4, 0));

    auto centreArea = area.reduced (40);
    centralDisplay.setBounds (centreArea);

    // Perillas superpuestas en las esquinas superiores (sobre los picos del display central)
    int knobSize = 64;
    speedKnob.setBounds (area.getX(), area.getY(), knobSize, knobSize);
    speedLabel.setBounds (area.getX(), area.getY() + knobSize, knobSize, 16);

    mixKnob.setBounds (area.getRight() - knobSize, area.getY(), knobSize, knobSize);
    mixLabel.setBounds (area.getRight() - knobSize, area.getY() + knobSize, knobSize, 16);
}
