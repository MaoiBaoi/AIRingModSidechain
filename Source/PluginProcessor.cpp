#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RingModProcessor::RingModProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",     juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output",    juce::AudioChannelSet::stereo(), true)
                          .withInput  ("Sidechain", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createLayout())
{
    mixParam  = apvts.getRawParameterValue ("mix");
    gainParam = apvts.getRawParameterValue ("gain");
}

juce::AudioProcessorValueTreeState::ParameterLayout RingModProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 },
        "Wet/Dry",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "gain", 1 },
        "Volume",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    return layout;
}

//==============================================================================
void RingModProcessor::prepareToPlay (double sampleRate, int)
{
    mixSmoothed.reset (sampleRate, 0.03);
    gainSmoothed.reset (sampleRate, 0.03);
    mixSmoothed.setCurrentAndTargetValue (mixParam->load() * 0.01f);
    gainSmoothed.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (gainParam->load()));
    sidechainLevel.store (0.0f);
}

bool RingModProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainIn  = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();

    // Eingang und Ausgang muessen gleich sein, Mono oder Stereo
    if (mainIn != mainOut)
        return false;

    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    // Sidechain: aus, Mono oder Stereo
    const auto& sc = layouts.getChannelSet (true, 1);

    if (! sc.isDisabled() && sc != juce::AudioChannelSet::mono() && sc != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

//==============================================================================
void RingModProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto mainBuffer = getBusBuffer (buffer, true, 0);   // Haupteingang (= Ausgang, in-place)
    auto scBuffer   = getBusBuffer (buffer, true, 1);   // Sidechain-Eingang

    const int numSamples  = buffer.getNumSamples();
    const int numMainCh   = juce::jmin (2, mainBuffer.getNumChannels());

    auto* scBus = getBus (true, 1);
    const bool scEnabled = scBus != nullptr && scBus->isEnabled();
    const int  numScCh   = scEnabled ? juce::jmin (2, scBuffer.getNumChannels()) : 0;

    float*       mainPtr[2] = { nullptr, nullptr };
    const float* scPtr[2]   = { nullptr, nullptr };

    for (int ch = 0; ch < numMainCh; ++ch)
        mainPtr[ch] = mainBuffer.getWritePointer (ch);

    for (int ch = 0; ch < numScCh; ++ch)
        scPtr[ch] = scBuffer.getReadPointer (ch);

    mixSmoothed.setTargetValue (mixParam->load() * 0.01f);
    gainSmoothed.setTargetValue (juce::Decibels::decibelsToGain (gainParam->load()));

    float scPeak = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        const float mix  = mixSmoothed.getNextValue();
        const float gain = gainSmoothed.getNextValue();

        for (int ch = 0; ch < numMainCh; ++ch)
        {
            const float dry = mainPtr[ch][i];
            float wet = dry;   // Ohne aktiven Sidechain: Signal unveraendert durchreichen

            if (numScCh > 0)
            {
                // Mono-Sidechain wird auf beide Kanaele verteilt
                const float carrier = scPtr[juce::jmin (ch, numScCh - 1)][i];
                scPeak = juce::jmax (scPeak, std::abs (carrier));

                wet = dry * carrier;   // <-- die eigentliche Ringmodulation
            }

            mainPtr[ch][i] = (dry + (wet - dry) * mix) * gain;
        }
    }

    // Pegelanzeige: schnell hoch, langsam runter
    sidechainLevel.store (juce::jmax (scPeak, sidechainLevel.load() * 0.9f));
}

//==============================================================================
juce::AudioProcessorEditor* RingModProcessor::createEditor()
{
    return new RingModEditor (*this);
}

void RingModProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void RingModProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// Wird vom Host aufgerufen, um das Plugin zu erzeugen
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingModProcessor();
}
