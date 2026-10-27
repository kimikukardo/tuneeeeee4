#pragma once
#include <JuceHeader.h>

// Piel visual inspirada en el Omnitrix: negro/gris metalico con acentos verdes.
class LookAndFeelOmni : public juce::LookAndFeel_V4
{
public:
    LookAndFeelOmni();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                        int buttonX, int buttonY, int buttonW, int buttonH,
                        juce::ComboBox&) override;

    static const juce::Colour chassisBlack;
    static const juce::Colour spikeGrey;
    static const juce::Colour omniGreen;
    static const juce::Colour capWhite;
};
