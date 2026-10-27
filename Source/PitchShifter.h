#pragma once
#include <vector>

// Corrector de tono en tiempo real, estilo PSOLA (Pitch Synchronous Overlap-Add).
// A diferencia de un simple cambio de velocidad de reproduccion, PSOLA conserva mejor
// el timbre/formantes original, por eso suena menos "robotico" que una correccion dura.
//
// El parametro "speed" controla que tan rapido el pitch corregido llega a la nota objetivo:
// valores bajos = deslizamiento audible y natural (estilo Waves), valores altos = correccion
// casi instantanea (mas parecido al efecto clasico de Antares).
class PitchShifter
{
public:
    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    void setPitchPeriod (float periodSamples); // periodo del pitch detectado, en muestras
    void setPitchRatio (float ratio);          // freqObjetivo / freqDetectada (1.0 = sin cambio)
    void setSpeed (float speedPercent);        // 0-100, ver explicacion arriba

    // Procesa un bloque mono in-place
    void process (float* samples, int numSamples);

private:
    double sampleRate = 44100.0;

    // Historial circular de entrada (de aqui se toman los "granos" hacia atras en el tiempo)
    std::vector<float> history;
    int historyWriteIdx = 0;
    static constexpr int historySizeSamples = 1 << 16; // ~1.5s a 44.1kHz

    // Anillo de salida (acumulacion overlap-add)
    std::vector<float> outputRing;
    std::vector<float> outputWeight; // suma de ventanas, para normalizar la mezcla
    int outputWriteIdx = 0;
    static constexpr int outputRingSize = 1 << 14;

    float currentPeriod = 220.0f;
    float targetPeriod = 220.0f;
    float currentRatio = 1.0f;
    float targetRatio = 1.0f;
    float smoothingRate = 0.0015f;

    double nextSynthesisMark = 0.0;

    void emitGrain (double sourcePosInHistory, int grainLength);
    static float hann (float x); // x en [0,1)
};
