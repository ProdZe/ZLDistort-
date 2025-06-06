﻿#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

class ZLDistortV2AudioProcessor : public juce::AudioProcessor
{
public:
    ZLDistortV2AudioProcessor();
    ~ZLDistortV2AudioProcessor() override;

    enum DistortionType
    {
        HardClip = 0,
        Foldback,
        Exponential,
        BitCrush,
        Wavefold,
        Harmonic
    };
    // new parameters
    std::atomic<float>* rootNoteParam = nullptr;   // MIDI note number 0–127
    std::atomic<float>* scaleMinorParam = nullptr;   // 0 = major, 1 = minor
    std::atomic<float>* numBandsParam = nullptr;   // e.g. 1–20 bands
    std::atomic<float>* bandQParam = nullptr;   // 0.1–10.0
    std::atomic<float>* softClipParam = nullptr;   // 0 = off, 1 = on

        // soft‑clip processor
    juce::dsp::Limiter<float> softLimiter;

    void prepareToPlay(double, int) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout&) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void doHarmonicDistortion(juce::AudioBuffer<float>&, int, float, float);

    std::vector<juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>> filters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZLDistortV2AudioProcessor)
};
