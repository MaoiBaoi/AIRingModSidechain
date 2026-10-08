#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
// Ringmodulator mit Sidechain-Eingang:
//   Haupteingang (Signal)  x  Sidechain-Eingang (Traeger)  =  Ausgang
//==============================================================================
class RingModProcessor : public juce::AudioProcessor
{
public:
    RingModProcessor();
    ~RingModProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    using AudioProcessor::processBlock;   // verhindert Warnung wegen der double-Variante
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "RingModSidechain"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameter (Regler)
    juce::AudioProcessorValueTreeState apvts;

    // Pegel am Sidechain-Eingang (nur fuer die Statusanzeige im Fenster)
    std::atomic<float> sidechainLevel { 0.0f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    std::atomic<float>* mixParam  = nullptr;   // 0..100 %
    std::atomic<float>* gainParam = nullptr;   // dB

    juce::SmoothedValue<float> mixSmoothed;
    juce::SmoothedValue<float> gainSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModProcessor)
};
