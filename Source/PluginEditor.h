/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ZLDistortV2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ZLDistortV2AudioProcessorEditor (ZLDistortV2AudioProcessor&);
    ~ZLDistortV2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ZLDistortV2AudioProcessor& audioProcessor;

    juce::Slider distortionSlider;
    juce::Slider dryWetSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZLDistortV2AudioProcessorEditor)
};
