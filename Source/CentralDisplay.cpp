#include "CentralDisplay.h"
#include "LookAndFeelOmni.h"
#include "NoteUtils.h"
#include <cmath>

CentralDisplay::CentralDisplay (std::function<float()> hzSource) : getDetectedHz (std::move (hzSource))
{
    startTimerHz (20);
}

void CentralDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto centre = bounds.getCentre();
    auto outerRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    // --- 4 picos diagonales (silueta inspirada en el Omnitrix) ---
    for (int i = 0; i < 4; ++i)
    {
        float angle = juce::MathConstants<float>::pi * 0.25f + (float) i * juce::MathConstants<float>::halfPi;
        float innerR = outerRadius * 0.55f;
        float outerR = outerRadius * 1.02f;

        juce::Point<float> base1 = centre.getPointOnCircumference (innerR, angle - 0.18f);
        juce::Point<float> base2 = centre.getPointOnCircumference (innerR, angle + 0.18f);
        juce::Point<float> tip   = centre.getPointOnCircumference (outerR, angle);

        juce::Path spike;
        spike.startNewSubPath (base1);
        spike.lineTo (tip);
        spike.lineTo (base2);
        spike.closeSubPath();

        g.setColour (LookAndFeelOmni::spikeGrey);
        g.fillPath (spike);
        g.setColour (LookAndFeelOmni::chassisBlack);
        g.strokePath (spike, juce::PathStrokeType (1.5f));

        // Punta blanca redondeada
        float capR = outerRadius * 0.11f;
        g.setColour (LookAndFeelOmni::capWhite);
        g.fillEllipse (tip.x - capR, tip.y - capR, capR * 2.0f, capR * 2.0f);
    }

    // --- Anillo central negro ---
    float ringR = outerRadius * 0.6f;
    g.setColour (LookAndFeelOmni::chassisBlack);
    g.fillEllipse (centre.x - ringR, centre.y - ringR, ringR * 2.0f, ringR * 2.0f);
    g.setColour (LookAndFeelOmni::spikeGrey);
    g.drawEllipse (centre.x - ringR, centre.y - ringR, ringR * 2.0f, ringR * 2.0f, 3.0f);

    // --- Simbolo verde en cruz ---
    float symR = ringR * 0.72f;
    g.setColour (LookAndFeelOmni::omniGreen);
    g.fillEllipse (centre.x - symR, centre.y - symR, symR * 2.0f, symR * 2.0f);

    juce::Path cross;
    cross.addRectangle (centre.x - symR, centre.y - symR * 0.16f, symR * 2.0f, symR * 0.32f);
    cross.addRectangle (centre.x - symR * 0.16f, centre.y - symR, symR * 0.32f, symR * 2.0f);
    g.setColour (LookAndFeelOmni::chassisBlack);
    g.fillPath (cross);

    // --- 4 tornillos verdes pequenos, como en la imagen de referencia ---
    for (int i = 0; i < 4; ++i)
    {
        float angle = (float) i * juce::MathConstants<float>::halfPi;
        auto p = centre.getPointOnCircumference (ringR * 0.86f, angle);
        g.setColour (LookAndFeelOmni::omniGreen);
        g.fillEllipse (p.x - 4.0f, p.y - 4.0f, 8.0f, 8.0f);
        g.setColour (LookAndFeelOmni::chassisBlack);
        g.drawEllipse (p.x - 4.0f, p.y - 4.0f, 8.0f, 8.0f, 1.0f);
    }

    // --- Nota detectada en vivo ---
    float hz = getDetectedHz ? getDetectedHz() : 0.0f;
    if (hz > 20.0f)
    {
        int midi = (int) std::round (NoteUtils::freqToMidi (hz));
        int noteIdx = ((midi % 12) + 12) % 12;
        int octave = midi / 12 - 1;
        currentNoteLabel = juce::String (NoteUtils::noteNames[(size_t) noteIdx]) + juce::String (octave);
    }
    else
    {
        currentNoteLabel = "--";
    }

    g.setColour (LookAndFeelOmni::capWhite);
    g.setFont (juce::Font (symR * 0.85f, juce::Font::bold));
    g.drawText (currentNoteLabel, bounds, juce::Justification::centred);
}
