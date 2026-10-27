#include "PitchShifter.h"
#include <algorithm>
#include <cmath>

void PitchShifter::prepare (double newSampleRate, int /*maxBlockSize*/)
{
    sampleRate = newSampleRate;
    history.assign (historySizeSamples, 0.0f);
    outputRing.assign (outputRingSize, 0.0f);
    outputWeight.assign (outputRingSize, 0.0f);
    reset();
}

void PitchShifter::reset()
{
    std::fill (history.begin(), history.end(), 0.0f);
    std::fill (outputRing.begin(), outputRing.end(), 0.0f);
    std::fill (outputWeight.begin(), outputWeight.end(), 0.0f);
    historyWriteIdx = 0;
    outputWriteIdx = 0;
    nextSynthesisMark = 0.0;
    currentPeriod = targetPeriod = (float) (sampleRate / 200.0);
    currentRatio = targetRatio = 1.0f;
}

void PitchShifter::setPitchPeriod (float periodSamples)
{
    if (periodSamples > 0.0f)
        targetPeriod = std::clamp (periodSamples, (float) (sampleRate / 1000.0), (float) (sampleRate / 50.0));
}

void PitchShifter::setPitchRatio (float ratio)
{
    targetRatio = std::clamp (ratio, 0.5f, 2.0f);
}

void PitchShifter::setSpeed (float speedPercent)
{
    float t = std::clamp (speedPercent, 0.0f, 100.0f) / 100.0f;
    // Mapeo no lineal: a velocidades bajas el deslizamiento hacia la nota es bien audible
    // (estilo "natural" tipo Waves); a velocidades altas se acerca a una correccion casi
    // instantanea (estilo clasico de Antares).
    smoothingRate = 0.00005f + t * t * 0.02f;
}

float PitchShifter::hann (float x)
{
    return 0.5f - 0.5f * std::cos (2.0f * (float) M_PI * x);
}

void PitchShifter::emitGrain (double sourcePosInHistory, int grainLength)
{
    for (int n = 0; n < grainLength; ++n)
    {
        double srcIdxD = sourcePosInHistory + n;
        int srcIdx0 = ((int) std::floor (srcIdxD) % historySizeSamples + historySizeSamples) % historySizeSamples;
        int srcIdx1 = (srcIdx0 + 1) % historySizeSamples;
        float frac = (float) (srcIdxD - std::floor (srcIdxD));
        float sample = history[(size_t) srcIdx0] * (1.0f - frac) + history[(size_t) srcIdx1] * frac;

        float w = hann ((float) n / (float) grainLength);

        int outIdx = (outputWriteIdx + n) % outputRingSize;
        outputRing[(size_t) outIdx] += sample * w;
        outputWeight[(size_t) outIdx] += w;
    }
}

void PitchShifter::process (float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        // 1. Guardamos la muestra entrante en el historial
        history[(size_t) historyWriteIdx] = samples[i];

        // 2. Suavizamos periodo y ratio hacia el objetivo (evita saltos bruscos / clicks).
        //    Esto es lo que le da el caracter "no robotico" al efecto: la correccion
        //    desliza hacia la nota en vez de saltar de golpe.
        currentPeriod += (targetPeriod - currentPeriod) * smoothingRate;
        currentRatio  += (targetRatio  - currentRatio)  * smoothingRate;

        // 3. Disparamos un nuevo grano cuando toca (sintesis pitch-synchronous)
        if (nextSynthesisMark <= 0.0)
        {
            int grainLen = (int) std::round (currentPeriod * 2.0f);
            grainLen = std::clamp (grainLen, 32, outputRingSize / 2);

            double sourcePos = (double) historyWriteIdx - grainLen * 0.5 - currentPeriod;
            emitGrain (sourcePos, grainLen);

            nextSynthesisMark = currentPeriod / std::max (0.01f, currentRatio);
        }
        nextSynthesisMark -= 1.0;

        // 4. Leemos la salida ya mezclada (overlap-add normalizado) y avanzamos el anillo
        float w = outputWeight[(size_t) outputWriteIdx];
        float outSample = w > 1.0e-6f ? outputRing[(size_t) outputWriteIdx] / w : 0.0f;

        outputRing[(size_t) outputWriteIdx] = 0.0f;
        outputWeight[(size_t) outputWriteIdx] = 0.0f;

        samples[i] = outSample;

        historyWriteIdx = (historyWriteIdx + 1) % historySizeSamples;
        outputWriteIdx = (outputWriteIdx + 1) % outputRingSize;
    }
}
