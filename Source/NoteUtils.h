#pragma once
#include <array>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace NoteUtils
{
    static const std::array<std::string, 12> noteNames = {
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
    };

    enum class ScaleType
    {
        Chromatic = 0,
        Major,
        NaturalMinor,
        HarmonicMinor,
        MajorPentatonic,
        MinorPentatonic
    };

    // Semitonos desde la tonica, dentro de una octava
    inline std::vector<int> scaleSemitones (ScaleType type)
    {
        switch (type)
        {
            case ScaleType::Chromatic:       return { 0,1,2,3,4,5,6,7,8,9,10,11 };
            case ScaleType::Major:           return { 0,2,4,5,7,9,11 };
            case ScaleType::NaturalMinor:    return { 0,2,3,5,7,8,10 };
            case ScaleType::HarmonicMinor:   return { 0,2,3,5,7,8,11 };
            case ScaleType::MajorPentatonic: return { 0,2,4,7,9 };
            case ScaleType::MinorPentatonic: return { 0,3,5,7,10 };
        }
        return { 0,2,4,5,7,9,11 };
    }

    // Frecuencia (Hz) -> nota MIDI (con decimales, para afinacion fina)
    inline double freqToMidi (double freqHz)
    {
        return 69.0 + 12.0 * std::log2 (freqHz / 440.0);
    }

    inline double midiToFreq (double midiNote)
    {
        return 440.0 * std::pow (2.0, (midiNote - 69.0) / 12.0);
    }

    // Dada una nota MIDI (con decimales), la tonica (0=C..11=B) y la escala,
    // devuelve la nota MIDI entera mas cercana que pertenezca a esa escala.
    inline double nearestInScale (double midiNote, int keyRoot, ScaleType scale)
    {
        auto semis = scaleSemitones (scale);
        double best = midiNote;
        double bestDist = 1.0e9;

        int baseOctave = (int) std::floor (midiNote / 12.0) - 1;
        for (int oct = baseOctave; oct <= baseOctave + 2; ++oct)
        {
            for (int s : semis)
            {
                double candidate = (double) (oct * 12 + keyRoot + s);
                double dist = std::abs (candidate - midiNote);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = candidate;
                }
            }
        }
        return best;
    }
}
