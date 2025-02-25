/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ZLDistortV2AudioProcessorEditor::ZLDistortV2AudioProcessorEditor (ZLDistortV2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    // Create the gain slider and attach it to the Gain parameter
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gainSlider.setRange(0.0, 10.0);
    addAndMakeVisible(gainSlider);

    // Attach the slider to the GAIN parameter
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "GAIN", gainSlider);

    setSize (400, 300);
}

ZLDistortV2AudioProcessorEditor::~ZLDistortV2AudioProcessorEditor()
{
}

//==============================================================================
void ZLDistortV2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (25.0f));
    g.drawFittedText ("GAIN", 0, 70, getWidth(), 20, juce::Justification::centred, 1);
}

void ZLDistortV2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // Position the gain slider in the middle
    gainSlider.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 50, 100, 100);
}
