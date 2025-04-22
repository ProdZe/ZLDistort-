#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
ZLDistortV2AudioProcessor::ZLDistortV2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

ZLDistortV2AudioProcessor::~ZLDistortV2AudioProcessor() {}

//==============================================================================
const juce::String ZLDistortV2AudioProcessor::getName() const { return JucePlugin_Name; }
bool ZLDistortV2AudioProcessor::acceptsMidi() const { return JucePlugin_WantsMidiInput; }
bool ZLDistortV2AudioProcessor::producesMidi() const { return JucePlugin_ProducesMidiOutput; }
bool ZLDistortV2AudioProcessor::isMidiEffect() const { return JucePlugin_IsMidiEffect; }
double ZLDistortV2AudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ZLDistortV2AudioProcessor::getNumPrograms() { return 1; }
int ZLDistortV2AudioProcessor::getCurrentProgram() { return 0; }
void ZLDistortV2AudioProcessor::setCurrentProgram(int) {}
const juce::String ZLDistortV2AudioProcessor::getProgramName(int) { return {}; }
void ZLDistortV2AudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void ZLDistortV2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    std::vector<float> freqs = {
        155.56f, 174.61f, 207.65f, 233.08f, 277.18f,
        311.13f, 349.23f, 415.30f, 466.16f, 554.37f,
        622.25f, 698.46f, 830.61f, 932.33f, 1108.73f,
        1244.51f, 1396.91f, 1661.22f, 1864.66f, 2217.46f
    };

    filters.clear();

    juce::dsp::ProcessSpec spec{
        sampleRate,
        static_cast<juce::uint32> (samplesPerBlock),
        static_cast<juce::uint32> (getTotalNumInputChannels())
    };

    for (auto freq : freqs)
    {
        filters.push_back({
            juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, freq, 2.0f)
            });
        filters.back().prepare(spec);
    }
}

void ZLDistortV2AudioProcessor::releaseResources() {}

bool ZLDistortV2AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // only allow mono or stereo, and require input == output
    const auto& outSet = layouts.getMainOutputChannelSet();
    const auto& inSet = layouts.getMainInputChannelSet();

    if ((outSet == juce::AudioChannelSet::mono() ||
        outSet == juce::AudioChannelSet::stereo())
        && inSet == outSet)
    {
        return true;
    }

    return false;
}


void ZLDistortV2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float dryWet = parameters.getRawParameterValue("DRYWET")->load();
    float distortionAmount = parameters.getRawParameterValue("DISTORTION")->load();
    if (distortionAmount < 0.01f) distortionAmount = 0.02f;

    int distortionMode = int(parameters.getRawParameterValue("DISTORTION_MODE")->load());

    if (distortionMode == DistortionType::Harmonic)
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
            doHarmonicDistortion(buffer, ch, distortionAmount, dryWet);
        return;
    }

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            float dry = data[s];
            float shaped = dry;

            switch (distortionMode)
            {
            case DistortionType::HardClip:
                shaped = juce::jlimit(-0.5f, 0.5f, dry * distortionAmount);
                break;
            case DistortionType::Foldback:
                if (dry > 0.5f || dry < -0.5f)
                    shaped = std::abs(std::fmod(dry - 0.5f, 1.0f) - 0.5f);
                shaped *= distortionAmount;
                break;
            case DistortionType::Exponential:
                shaped = std::copysign(1.0f, dry)
                    * (1.0f - std::exp(-std::abs(dry * distortionAmount)));
                break;
            case DistortionType::BitCrush:
                shaped = std::round(dry * 8.0f) / 8.0f;
                break;
            case DistortionType::Wavefold:
                if (dry > 0.5f)      shaped = 1.0f - (dry - 0.5f);
                else if (dry < -0.5f) shaped = -1.0f - (dry + 0.5f);
                shaped *= distortionAmount;
                break;
            default:
                break;
            }

            data[s] = dry * (1.0f - dryWet) + shaped * dryWet;
        }
    }
}

//==============================================================================
bool ZLDistortV2AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ZLDistortV2AudioProcessor::createEditor() { return new ZLDistortV2AudioProcessorEditor(*this); }
void ZLDistortV2AudioProcessor::getStateInformation(juce::MemoryBlock&) {}
void ZLDistortV2AudioProcessor::setStateInformation(const void*, int) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ZLDistortV2AudioProcessor(); }

juce::AudioProcessorValueTreeState::ParameterLayout ZLDistortV2AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DISTORTION", "Distortion", 0.0f, 10.0f, 5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DRYWET", "Dry/Wet", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "DISTORTION_MODE", "Distortion Mode",
        juce::StringArray{ "Hard Clip", "Foldback", "Exponential", "Bit Crush", "Wavefold", "Harmonic" },
        0));

    return { params.begin(), params.end() };
}

void ZLDistortV2AudioProcessor::doHarmonicDistortion(juce::AudioBuffer<float>& buffer,
    int channel,
    float distortionAmount,
    float dryWet)
{
    if (filters.empty()) return;
    auto numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> harmBuf(1, numSamples);
    harmBuf.clear();

    for (auto& f : filters)
    {
        juce::AudioBuffer<float> tmp(1, numSamples);
        tmp.copyFrom(0, 0, buffer, channel, 0, numSamples);

        juce::dsp::AudioBlock<float> block(tmp);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        f.process(ctx);

        for (int i = 0; i < numSamples; ++i)
        {
            auto x = tmp.getSample(0, i);
            auto d = std::copysign(1.0f, x)
                * (1.0f - std::exp(-std::abs(x * distortionAmount)));
            harmBuf.addSample(0, i, d);
        }
    }

    auto numBands = (float)filters.size();
    for (int i = 0; i < numSamples; ++i)
    {
        float d = buffer.getSample(channel, i);
        float h = harmBuf.getSample(0, i) / numBands;
        buffer.setSample(channel, i, d * (1.0f - dryWet) + h * dryWet);
    }
}
