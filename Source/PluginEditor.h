#pragma once

#include "PluginProcessor.h"

//==============================================================================
class RingModEditor : public juce::AudioProcessorEditor,
                      private juce::Timer
{
public:
    explicit RingModEditor (RingModProcessor&);
    ~RingModEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void setupKnob (juce::Slider&, const juce::String& suffix, int decimals);

    RingModProcessor& processor;

    juce::Slider mixSlider, gainSlider;
    juce::Label  mixLabel, gainLabel, statusLabel;

    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModEditor)
};
