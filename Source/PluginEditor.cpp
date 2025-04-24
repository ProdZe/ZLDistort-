#include "PluginEditor.h"

ZLDistortV2AudioProcessorEditor::ZLDistortV2AudioProcessorEditor(ZLDistortV2AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(650, 370);

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
       {
           auto limiterX = modeBox.getRight() + 20;
           auto limiterY = modeBox.getY();
           softClipLabel.setBounds(limiterX, limiterY, 50, 20);
           softClipToggle.setBounds(limiterX + 50, limiterY, 20, 20);
       }

    //–– Attach controls to the parameters ––
    distortionAttach = std::make_unique<SliderAttachment>(
        processorRef.parameters, "DISTORTION", distortionSlider);
    dryWetAttach = std::make_unique<SliderAttachment>(
        processorRef.parameters, "DRYWET", dryWetSlider);
    modeAttach = std::make_unique<ChoiceAttachment>(
        processorRef.parameters, "DISTORTION_MODE", modeBox);
    
    //Re-layout when mode changes to harmonic
    modeBox.onChange = [this] { resized(); };

    //–– Harmonic‑mode controls ––
 // Root Note
    rootNoteLabel.setText("Root Note", juce::dontSendNotification);
    rootNoteLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(rootNoteLabel);

    // build a fixed 12‑note list instead of calling getMidiNoteNames()
    juce::StringArray midiNotes{ "C", "C#", "D", "D#", "E", "F",
                                 "F#", "G", "G#", "A", "A#", "B" };
    rootNoteBox.addItemList(midiNotes, 1);
    addAndMakeVisible(rootNoteBox);

    rootNoteBox.onChange = [this]
        {
            // map 1–12 to 0.0–1.0
            auto val = (rootNoteBox.getSelectedId() - 1) / 11.0f;
            processorRef.parameters.getParameter("ROOT_NOTE")
                ->setValueNotifyingHost(val);
        };

    // Scale Type
    scaleTypeLabel.setText("Scale", juce::dontSendNotification);
    scaleTypeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(scaleTypeLabel);
    scaleTypeBox.addItem("Major", 1);
    scaleTypeBox.addItem("Minor", 2);
    addAndMakeVisible(scaleTypeBox);
    scaleTypeBox.onChange = [this]
        {
            processorRef.parameters.getParameter("SCALE_MINOR")
                ->setValueNotifyingHost((scaleTypeBox.getSelectedId() - 1));
        };

    // Number of Bands
    numBandsLabel.setText("Bands", juce::dontSendNotification);
    numBandsLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(numBandsLabel);
    numBandsSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    numBandsSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    numBandsSlider.setRange(1, 20, 1);
    addAndMakeVisible(numBandsSlider);
    numBandsAttachment.reset(new Attachment(processorRef.parameters, "NUM_BANDS", numBandsSlider));

    // Q
    qLabel.setText("Q", juce::dontSendNotification);
    qLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(qLabel);
    qSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    qSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    qSlider.setRange(0.1, 10.0, 0.01);
    addAndMakeVisible(qSlider);
    qAttachment.reset(new Attachment(processorRef.parameters, "BAND_Q", qSlider));

    // Soft‑Clip Toggle
    softClipLabel.setText("Limit", juce::dontSendNotification);
    softClipLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(softClipLabel);
    addAndMakeVisible(softClipToggle);
    softClipAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
        processorRef.parameters, "SOFT_CLIP", softClipToggle));
}



ZLDistortV2AudioProcessorEditor::~ZLDistortV2AudioProcessorEditor() = default;

void ZLDistortV2AudioProcessorEditor::paint(juce::Graphics& g)
{
   // 2) set up a linear gradient from top‑left to bottom‑right
    juce::Colour c1(30, 30, 30), c2(10, 10, 10);
    g.fillAll(juce::Colour::fromRGB(20, 20, 20));
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
    auto area = getLocalBounds().reduced(20);

    // --- Title space (just leave it blank here) ---
    area.removeFromTop(40);

    // --- Knob row ---
    auto knobsRow = area.removeFromTop(140);
    constexpr int knobSize = 120;

    distortionSlider.setBounds(knobsRow.removeFromLeft(knobSize));
    distortionLabel.setBounds(distortionSlider.getX(),
        distortionSlider.getY() - 20,
        knobSize, 20);

    knobsRow.removeFromLeft(40);  // spacer

    dryWetSlider.setBounds(knobsRow.removeFromRight(knobSize));
    dryWetLabel.setBounds(dryWetSlider.getX(),
        dryWetSlider.getY() - 20,
        knobSize, 20);

    // --- Mode row ---
    area.removeFromTop(20);
    auto modeRow = area.removeFromTop(50);

    modeBox.setBounds(modeRow.withSizeKeepingCentre(200, 30));
    modeLabel.setBounds(modeBox.getX(),
        modeBox.getY() - 25,
        modeBox.getWidth(),
        20);

    // --- Limiter (always visible) ---
    const int limLabW = 70, limH = 20;
    softClipLabel.setBounds(modeBox.getRight() + 20,
        modeBox.getY(),
        limLabW, limH);
    softClipToggle.setBounds(modeBox.getRight() + 20 + limLabW,
        modeBox.getY(),
        limH, limH);

    // --- Harmonic‑mode extras in the bottom leftover area ---
    bool isH = (processorRef.parameters
        .getRawParameterValue("DISTORTION_MODE")->load()
        == (float)ZLDistortV2AudioProcessor::Harmonic);

    rootNoteLabel.setVisible(isH);
    rootNoteBox.setVisible(isH);
    scaleTypeLabel.setVisible(isH);
    scaleTypeBox.setVisible(isH);
    numBandsLabel.setVisible(isH);
    numBandsSlider.setVisible(isH);
    qLabel.setVisible(isH);
    qSlider.setVisible(isH);

    if (isH)
    {
        // everything left in `area` now is the bottom strip
        auto extras = area;
        extras.reduce(0, 10); // top/bottom margin

        const int rowH = 24;
        int y0 = extras.getY();

        // Root Note (left)
        rootNoteLabel.setBounds(extras.getX(), y0, 80, rowH);
        rootNoteBox.setBounds(extras.getX(), y0 + rowH, 100, rowH);
        rootNoteLabel.setJustificationType(juce::Justification::centred);

        // Scale (right)
       
        scaleTypeLabel.setBounds(extras.getRight() - 60, y0, 90, rowH);
        
        scaleTypeBox.setBounds(extras.getRight() - 100, y0 + rowH, 95, rowH);
       

        // Bands & Q (centered)
        int totalW = 50 + 120 + 20 + 120;
        int cx = getWidth() / 2 - totalW / 2;
        numBandsLabel.setBounds(cx, y0, 50, rowH);
        numBandsSlider.setBounds(cx + 50, y0, 120, rowH);
        qLabel.setBounds(cx + 50 + 120, y0, 20, rowH);
        qSlider.setBounds(cx + 50 + 120 + 20, y0, 120, rowH);
    }
}