#pragma once
#include <vector>

// Detector de tono (pitch) monofonico usando el algoritmo YIN.
// Pensado para voz: devuelve 0 cuando no hay un tono claro (silencio, consonantes, ruido).
class PitchDetector
{
public:
    void prepare (double sampleRate, int analysisWindowSize);

    // samples/numSamples: ventana de analisis (mono). Devuelve la frecuencia en Hz, o 0 si no hay pitch claro.
    float detectPitch (const float* samples, int numSamples);

private:
    double sampleRate = 44100.0;
    int windowSize = 1024;
    std::vector<float> diffBuffer;
    float yinThreshold = 0.15f;
};
