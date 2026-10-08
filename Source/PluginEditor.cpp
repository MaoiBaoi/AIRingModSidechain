#include "PluginEditor.h"

namespace
{
    const juce::Colour accent     { 0xffff8a3d };
    const juce::Colour background { 0xff1b1d22 };
}

//==============================================================================
RingModEditor::RingModEditor (RingModProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      mixAttachment  (p.apvts, "mix",  mixSlider),
      gainAttachment (p.apvts, "gain", gainSlider)
{
    setupKnob (mixSlider,  " %",  0);
    setupKnob (gainSlider, " dB", 1);

    mixSlider.setDoubleClickReturnValue  (true, 100.0);
    gainSlider.setDoubleClickReturnValue (true, 0.0);

    auto setupLabel = [this] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.85f));
        addAndMakeVisible (label);
    };

    setupLabel (mixLabel,  "WET / DRY");
    setupLabel (gainLabel, "VOLUME");

    statusLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (statusLabel);

    setSize (360, 280);
    startTimerHz (15);
    timerCallback();
}

RingModEditor::~RingModEditor()
{
    stopTimer();
}

void RingModEditor::setupKnob (juce::Slider& s, const juce::String& suffix, int decimals)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 22);
    s.setTextValueSuffix (suffix);
    s.setNumDecimalPlacesToDisplay (decimals);
    s.setColour (juce::Slider::rotarySliderFillColourId,    accent);
    s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff3a3d46));
    s.setColour (juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour (juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour (juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible (s);
}

//==============================================================================
void RingModEditor::paint (juce::Graphics& g)
{
    g.fillAll (background);

    g.setColour (accent);
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText ("RING MOD SIDECHAIN", getLocalBounds().removeFromTop (44), juce::Justification::centred);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawLine (20.0f, 44.0f, (float) getWidth() - 20.0f, 44.0f, 1.0f);
}

void RingModEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (48);

    statusLabel.setBounds (area.removeFromTop (26));
    area.removeFromBottom (12);

    auto left  = area.removeFromLeft (area.getWidth() / 2);
    auto right = area;

    mixLabel.setBounds  (left.removeFromTop (22));
    gainLabel.setBounds (right.removeFromTop (22));

    mixSlider.setBounds  (left.reduced (14, 0));
    gainSlider.setBounds (right.reduced (14, 0));
}

//==============================================================================
void RingModEditor::timerCallback()
{
    auto* sc = processor.getBus (true, 1);

    if (sc == nullptr || ! sc->isEnabled())
    {
        statusLabel.setText ("Sidechain: nicht aktiv", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe05555));
    }
    else if (processor.sidechainLevel.load() > 0.0005f)
    {
        statusLabel.setText ("Sidechain: Signal liegt an", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xff5fd075));
    }
    else
    {
        statusLabel.setText ("Sidechain: kein Signal", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe0b050));
    }
}
