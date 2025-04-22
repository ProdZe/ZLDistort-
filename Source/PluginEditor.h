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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZLDistortV2AudioProcessorEditor)
};
