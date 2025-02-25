/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
ZLDistortV2AudioProcessor::ZLDistortV2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
   
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
   
{
    
}

ZLDistortV2AudioProcessor::~ZLDistortV2AudioProcessor()
{
}

//==============================================================================
const juce::String ZLDistortV2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ZLDistortV2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ZLDistortV2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ZLDistortV2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ZLDistortV2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ZLDistortV2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ZLDistortV2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void ZLDistortV2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ZLDistortV2AudioProcessor::getProgramName (int index)
{
    return {};
}

void ZLDistortV2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ZLDistortV2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ZLDistortV2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ZLDistortV2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ZLDistortV2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
       
        // Retrieve parameter values inside the processBlock
        float distortionAmount = parameters.getRawParameterValue("DISTORTION")->load();
        float dryWet = parameters.getRawParameterValue("DRYWET")->load();
        // Prevent complete silence if distortionAmount is 0
        if (distortionAmount <= 0.01f)
            distortionAmount = 0.01f;

        //Distortion's code implamentation adds another for loop for samples

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float drySample = channelData[sample]; // Original signal
            float distortedSample = drySample;     // Default to dry sample
            int distortionMode = static_cast<int>(parameters.getRawParameterValue("DISTORTION_MODE")->load());

            // Apply distortion based on selected mode
            switch (distortionMode)
            {
            case 0: // Hard Clip
                distortedSample = juce::jlimit(-0.5f, 0.5f, drySample * distortionAmount);
                break;

            case 1: // Foldback Distortion
                if (drySample > 0.5f || drySample < -0.5f)
                    distortedSample = std::abs(std::fmod(drySample - 0.5f, 1.0f) - 0.5f);
                else
                    distortedSample = drySample;
                distortedSample *= distortionAmount;
                break;

            case 2: // Exponential Distortion
                distortedSample = std::copysign(1.0f, drySample) * (1 - std::exp(-std::abs(drySample * distortionAmount)));
                break;

            case 3: // Bit Crush
                distortedSample = std::round(drySample * 8.0f) / 8.0f;
                break;

            case 4: // Wavefold Distortion
                if (drySample > 0.5f)
                    distortedSample = 1.0f - (drySample - 0.5f);
                else if (drySample < -0.5f)
                    distortedSample = -1.0f - (drySample + 0.5f);
                else
                    distortedSample = drySample;
                distortedSample *= distortionAmount;
                break;

            default: // Fallback: No distortion
                distortedSample = drySample;
                break;
            }

            // Dry/Wet Mix
            channelData[sample] = (1.0f - dryWet) * drySample + dryWet * distortedSample;         
        }

    }
    // Clear any extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

//==============================================================================
bool ZLDistortV2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ZLDistortV2AudioProcessor::createEditor()
{
    return new ZLDistortV2AudioProcessorEditor (*this);
}

//==============================================================================
void ZLDistortV2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ZLDistortV2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZLDistortV2AudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout ZLDistortV2AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Distortion Amount Parameter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DISTORTION", "Distortion", 0.0f, 10.0f, 5.0f));

    // Dry/Wet Mix Parameter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DRYWET", "Dry/Wet", 0.0f, 1.0f, 0.5f)); // 0 = dry, 1 = wet

    // 🔄 Distortion Mode ComboBox
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "DISTORTION_MODE", "Distortion Mode", juce::StringArray{ "Hard Clip", "Foldback", "Exponential", "Bit Crush", "Wavefold" }, 0));

    return { params.begin(), params.end() };
}