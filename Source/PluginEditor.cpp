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

    // Create the slider and attach it to the distortion parameter
    distortionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    distortionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    distortionSlider.setRange(0.0, 10.0);
    addAndMakeVisible(distortionSlider);

    // Attach the slider to the distortion parameter
    distortionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "DISTORTION", distortionSlider);

    // Dry/Wet Slider
    dryWetSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(dryWetSlider);
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "DRYWET", dryWetSlider);

    // 🔄 ComboBox for Distortion Modes
    distortionModeBox.addItem("Hard Clip", 1);
    distortionModeBox.addItem("Foldback", 2);
    distortionModeBox.addItem("Exponential", 3);
    distortionModeBox.addItem("Bit Crush", 4);
    distortionModeBox.addItem("Wavefold", 5);
    addAndMakeVisible(distortionModeBox);

    // Attach ComboBox to ValueTreeState
    distortionModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "DISTORTION_MODE", distortionModeBox);

    setSize (600, 400);
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
    g.drawFittedText ("DISTORTION", 0, 70, getWidth(), 20, juce::Justification::centred, 1);

    g.setFont(juce::Font(15.0f));

    // "Dry" label to the left of Dry/Wet slider
    g.drawFittedText("Dry", 50, 250, 50, 20, juce::Justification::centred, 1);

    // "Wet" label to the right of Dry/Wet slider
    g.drawFittedText("Wet", getWidth() - 100, 250, 50, 20, juce::Justification::centred, 1);
}

void ZLDistortV2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // 📏 Position the sliders and dropdown
    distortionSlider.setBounds(50, 50, 300, 40);
    dryWetSlider.setBounds(50, 110, 300, 40);
    distortionModeBox.setBounds(235, 300, 120, 30); // Dropdown positioning

   // Position the sliders
    distortionSlider.setBounds(50, 100, getWidth() - 100, 50); // Distortion Amount
    dryWetSlider.setBounds(50, 200, getWidth() - 100, 50);     // Dry/Wet Mix
}
