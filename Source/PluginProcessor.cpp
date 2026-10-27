#include "PluginProcessor.h"
#include "PluginEditor.h"

RealtuneAudioProcessor::RealtuneAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                        .withOutput ("Output", juce::AudioChannelSet::mono(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout RealtuneAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray keyNames { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("key", "Key", keyNames, 0));

    juce::StringArray scaleNames { "Chromatic","Major","Natural Minor","Harmonic Minor","Major Pentatonic","Minor Pentatonic" };
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("scale", "Scale", scaleNames, 1));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "speed", "Retune Speed", juce::NormalisableRange<float> (0.0f, 100.0f), 35.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "mix", "Mix", juce::NormalisableRange<float> (0.0f, 100.0f), 100.0f));

    return { params.begin(), params.end() };
}

void RealtuneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    pitchDetector.prepare (sampleRate, analysisWindowSize);
    pitchShifter.prepare (sampleRate, samplesPerBlock);
    analysisWindow.assign (analysisWindowSize, 0.0f);
    analysisWritePos = 0;
}

bool RealtuneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void RealtuneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto* channelData = buffer.getWritePointer (0);
    const int numSamples = buffer.getNumSamples();

    // Copia "seca" para el control de mezcla (Mix)
    std::vector<float> dry (channelData, channelData + numSamples);

    // 1. Alimentamos la ventana de analisis para la deteccion de pitch
    for (int i = 0; i < numSamples; ++i)
    {
        analysisWindow[(size_t) analysisWritePos] = channelData[i];
        analysisWritePos = (analysisWritePos + 1) % analysisWindowSize;
    }

    float detectedHz = pitchDetector.detectPitch (analysisWindow.data(), analysisWindowSize);
    detectedFrequencyHz.store (detectedHz);

    int keyIndex   = (int) apvts.getRawParameterValue ("key")->load();
    int scaleIndex = (int) apvts.getRawParameterValue ("scale")->load();
    float speedPct = apvts.getRawParameterValue ("speed")->load();
    float mixPct   = apvts.getRawParameterValue ("mix")->load();

    pitchShifter.setSpeed (speedPct);

    if (detectedHz > 20.0f)
    {
        double midiIn = NoteUtils::freqToMidi ((double) detectedHz);
        double midiTarget = NoteUtils::nearestInScale (midiIn, keyIndex, (NoteUtils::ScaleType) scaleIndex);
        double targetHz = NoteUtils::midiToFreq (midiTarget);

        float ratio = (float) (targetHz / (double) detectedHz);
        pitchShifter.setPitchPeriod ((float) (getSampleRate() / (double) detectedHz));
        pitchShifter.setPitchRatio (ratio);
    }
    else
    {
        pitchShifter.setPitchRatio (1.0f);
    }

    pitchShifter.process (channelData, numSamples);

    // Mezcla dry/wet
    const float wet = mixPct / 100.0f;
    for (int i = 0; i < numSamples; ++i)
        channelData[i] = dry[(size_t) i] * (1.0f - wet) + channelData[i] * wet;
}

void RealtuneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
        if (auto xml = state.createXml())
            copyXmlToBinary (*xml, destData);
}

void RealtuneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* RealtuneAudioProcessor::createEditor()
{
    return new RealtuneAudioProcessorEditor (*this);
}

// JUCE necesita esta funcion global para instanciar el plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RealtuneAudioProcessor();
}
