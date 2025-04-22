#include "PluginEditor.h"

ZLDistortV2AudioProcessorEditor::ZLDistortV2AudioProcessorEditor(ZLDistortV2AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    //–– Distortion knob ––
    distortionSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(distortionSlider);
    distortionLabel.setText("Distortion", juce::dontSendNotification);
    addAndMakeVisible(distortionLabel);

    //–– Dry/Wet knob ––
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(dryWetSlider);
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    addAndMakeVisible(dryWetLabel);

    //–– Mode dropdown ––
    modeBox.addItemList(processorRef.parameters.getParameter("DISTORTION_MODE")->getAllValueStrings(), 1);
    addAndMakeVisible(modeBox);
    modeBox.setJustificationType(juce::Justification::centred);

       addAndMakeVisible(modeLabel);
       modeLabel.setText("Mode", juce::dontSendNotification);
       modeLabel.setJustificationType(juce::Justification::centred);

    //–– Attach controls to the parameters ––
    distortionAttach = std::make_unique<SliderAttachment>(
        processorRef.parameters, "DISTORTION", distortionSlider);
    dryWetAttach = std::make_unique<SliderAttachment>(
        processorRef.parameters, "DRYWET", dryWetSlider);
    modeAttach = std::make_unique<ChoiceAttachment>(
        processorRef.parameters, "DISTORTION_MODE", modeBox);

    setSize(360, 240);
}

ZLDistortV2AudioProcessorEditor::~ZLDistortV2AudioProcessorEditor() = default;

void ZLDistortV2AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(20, 20, 20));
    
    // 2) set up a linear gradient from top‑left to bottom‑right
    juce::Colour c1(30, 30, 30), c2(10, 10, 10);
    juce::ColourGradient gradient(c1,
        0.0f, 0.0f,          // start point
        c2,
        (float)getWidth(),
        (float)getHeight(), // end point
        false);              // not radial

    g.setGradientFill(gradient);

    // 3) fill the entire background with our gradient
    g.fillRect(getLocalBounds());

    //draw ZL distort label 
    g.setColour(juce::Colours::white);
    g.setFont(25.0f);
    g.drawFittedText("ZL-Distort",
        getLocalBounds().removeFromTop(25),
        juce::Justification::centred, 1);
}

void ZLDistortV2AudioProcessorEditor::resized()
{
    // overall margin
    auto area = getLocalBounds().reduced(15);

    // reserve 25px for the title at the very top
    area.removeFromTop(25);

    // top zone for knobs (100px high)
    auto knobsArea = area.removeFromTop(100);

    constexpr int kW = 100, kH = 100;

    // Distortion knob on the left edge
    distortionSlider.setBounds(knobsArea.getX(), knobsArea.getY(), kW, kH);
    distortionLabel.setBounds(distortionSlider.getX(),
        distortionSlider.getY() - 20,
        kW, 20);
    distortionLabel.setJustificationType(juce::Justification::centred);

    // Dry/Wet knob on the right edge
    dryWetSlider.setBounds(knobsArea.getRight() - kW,
        knobsArea.getY(), kW, kH);
    dryWetLabel.setBounds(dryWetSlider.getX(),
        dryWetSlider.getY() - 20,
        kW, 20);
    dryWetLabel.setJustificationType(juce::Justification::centred);

    // bottom zone for combo box
    auto bottom = area.removeFromBottom(50);

    // center the combo
    modeBox.setBounds(bottom.withSizeKeepingCentre(150, 30));
    modeLabel.setBounds(modeBox.getX(),
        modeBox.getY() - 40,
        modeBox.getWidth(),
        20);
}
