#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class ZLDistortV2AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ZLDistortV2AudioProcessorEditor(ZLDistortV2AudioProcessor&);
    ~ZLDistortV2AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ZLDistortV2AudioProcessor& processorRef;

    juce::Slider        distortionSlider, dryWetSlider;
    juce::Label         distortionLabel, dryWetLabel;
    juce::ComboBox      modeBox;
    juce::Label         modeLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ChoiceAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment>      distortionAttach, dryWetAttach;
    std::unique_ptr<ChoiceAttachment>      modeAttach;


    // harmonic‑mode only controls
    juce::ComboBox    rootNoteBox, scaleTypeBox;
    juce::Slider      numBandsSlider, qSlider;
    juce::ToggleButton softClipToggle;
    juce::Label       rootNoteLabel, scaleTypeLabel, numBandsLabel, qLabel, softClipLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment>   numBandsAttachment, qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> softClipAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZLDistortV2AudioProcessorEditor)
};
