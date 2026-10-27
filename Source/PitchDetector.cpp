#include "PitchDetector.h"
#include <algorithm>
#include <cmath>

void PitchDetector::prepare (double newSampleRate, int analysisWindowSize)
{
    sampleRate = newSampleRate;
    windowSize = analysisWindowSize;
    diffBuffer.assign ((size_t) windowSize / 2, 0.0f);
}

float PitchDetector::detectPitch (const float* samples, int numSamples)
{
    const int halfWindow = std::min (numSamples / 2, (int) diffBuffer.size());
    if (halfWindow < 32)
        return 0.0f;

    // Paso 1-2 de YIN: funcion de diferencia al cuadrado
    for (int tau = 0; tau < halfWindow; ++tau)
    {
        float sum = 0.0f;
        for (int i = 0; i < halfWindow; ++i)
        {
            float delta = samples[i] - samples[i + tau];
            sum += delta * delta;
        }
        diffBuffer[(size_t) tau] = sum;
    }

    // Paso 3: normalizacion acumulativa media
    diffBuffer[0] = 1.0f;
    float runningSum = 0.0f;
    for (int tau = 1; tau < halfWindow; ++tau)
    {
        runningSum += diffBuffer[(size_t) tau];
        diffBuffer[(size_t) tau] *= (float) tau / (runningSum > 0.0f ? runningSum : 1.0f);
    }

    // Paso 4: umbral absoluto (busca el primer minimo local por debajo del umbral)
    int tauEstimate = -1;
    for (int tau = 2; tau < halfWindow; ++tau)
    {
        if (diffBuffer[(size_t) tau] < yinThreshold)
        {
            while (tau + 1 < halfWindow && diffBuffer[(size_t) (tau + 1)] < diffBuffer[(size_t) tau])
                ++tau;
            tauEstimate = tau;
            break;
        }
    }

    if (tauEstimate == -1)
        return 0.0f; // no se detecto un tono claro (silencio, consonante, ruido)

    // Paso 5: interpolacion parabolica para afinar el periodo estimado
    float betterTau = (float) tauEstimate;
    if (tauEstimate > 0 && tauEstimate < halfWindow - 1)
    {
        float s0 = diffBuffer[(size_t) (tauEstimate - 1)];
        float s1 = diffBuffer[(size_t) tauEstimate];
        float s2 = diffBuffer[(size_t) (tauEstimate + 1)];
        float denom = 2.0f * (2.0f * s1 - s2 - s0);
        if (std::abs (denom) > 1.0e-9f)
            betterTau += (s2 - s0) / denom;
    }

    if (betterTau <= 0.0f)
        return 0.0f;

    return (float) (sampleRate / betterTau);
}
