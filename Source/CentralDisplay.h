#pragma once
#include <JuceHeader.h>
#include <functional>

// Dibuja el "reloj" central tipo Omnitrix: 4 picos diagonales con puntas blancas,
// anillo negro, simbolo en cruz verde, 4 tornillos verdes, y la nota detectada en vivo.
class CentralDisplay : public juce::Component, private juce::Timer
{
public:
    explicit CentralDisplay (std::function<float()> getDetectedHz);
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override { repaint(); }

    std::function<float()> getDetectedHz;
    juce::String currentNoteLabel { "--" };
};
