#include "LookAndFeelOmni.h"

const juce::Colour LookAndFeelOmni::chassisBlack { 0xff17181a };
const juce::Colour LookAndFeelOmni::spikeGrey    { 0xff4d5257 };
const juce::Colour LookAndFeelOmni::omniGreen    { 0xff8ed323 };
const juce::Colour LookAndFeelOmni::capWhite     { 0xfff1f1f1 };

LookAndFeelOmni::LookAndFeelOmni()
{
    setColour (juce::Slider::textBoxTextColourId, capWhite);
    setColour (juce::Slider::textBoxOutlineColourId, spikeGrey);
    setColour (juce::ComboBox::textColourId, omniGreen);
    setColour (juce::ComboBox::backgroundColourId, chassisBlack);
    setColour (juce::ComboBox::outlineColourId, spikeGrey);
    setColour (juce::Label::textColourId, capWhite);
}

void LookAndFeelOmni::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Cuerpo del knob: gris metalico, como las puntas del Omnitrix
    g.setColour (spikeGrey);
    g.fillEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
    g.setColour (chassisBlack);
    g.drawEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 2.0f);

    // Arco de valor, estilo "energia" verde
    juce::Path arc;
    arc.addCentredArc (centre.x, centre.y, radius - 4.0f, radius - 4.0f, 0.0f,
                        rotaryStartAngle, angle, true);
    g.setColour (omniGreen);
    g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Puntero blanco, como las tapas redondeadas del Omnitrix
    juce::Point<float> tip (centre.x + std::sin (angle) * (radius - 8.0f),
                             centre.y - std::cos (angle) * (radius - 8.0f));
    g.setColour (capWhite);
    g.fillEllipse (tip.x - 3.0f, tip.y - 3.0f, 6.0f, 6.0f);
}

void LookAndFeelOmni::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                     int, int, int, int, juce::ComboBox&)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height).reduced (1.0f);
    g.setColour (chassisBlack);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (omniGreen);
    g.drawRoundedRectangle (bounds, 4.0f, 1.5f);
}
