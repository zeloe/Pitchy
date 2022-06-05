/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PitchyAudioProcessor::PitchyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    initDsp();
}

PitchyAudioProcessor::~PitchyAudioProcessor()
{
}

//==============================================================================
const juce::String PitchyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PitchyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PitchyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PitchyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PitchyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PitchyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PitchyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PitchyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PitchyAudioProcessor::getProgramName (int index)
{
    return {};
}

void PitchyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PitchyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
        detunerOneL->getTheSampleRate(sampleRate);
    detunerOneL->delayLine1->prepare(spec);
    detunerOneL->delayLine2->prepare(spec);
        detunerOneR->getTheSampleRate(sampleRate);
    detunerOneR->delayLine1->prepare(spec);
    detunerOneR->delayLine2->prepare(spec);
        detunerTwoL->getTheSampleRate(sampleRate);
    detunerTwoL->delayLine1->prepare(spec);
    detunerTwoL->delayLine2->prepare(spec);
        detunerTwoR->getTheSampleRate(sampleRate);
    detunerTwoR->delayLine1->prepare(spec);
    detunerTwoR->delayLine2->prepare(spec);
        detunerThreeL->getTheSampleRate(sampleRate);
    detunerThreeL->delayLine1->prepare(spec);
    detunerThreeL->delayLine2->prepare(spec);
        detunerThreeR->getTheSampleRate(sampleRate);
    detunerThreeR->delayLine1->prepare(spec);
    detunerThreeR->delayLine2->prepare(spec);
        detunerFourL->getTheSampleRate(sampleRate);
    detunerFourL->delayLine1->prepare(spec);
    detunerFourL->delayLine2->prepare(spec);
        detunerFourR->getTheSampleRate(sampleRate);
    detunerFourR->delayLine1->prepare(spec);
    detunerFourR->delayLine2->prepare(spec);
    detuneSmooth.reset(sampleRate, 0.001f);
    dryWetSmooth.reset(sampleRate, 0.001f);
    ampSmooth.reset(sampleRate, 0.001f);
}

void PitchyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PitchyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PitchyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    detuneSmooth.setTargetValue(apvts.getRawParameterValue("Detune")->load());
    double detuneFreq = pfreq(detuneSmooth.getNextValue() * 0.5f,  100.f);
    dryWetSmooth.setTargetValue(apvts.getRawParameterValue("Wet")->load());
    ampSmooth.setTargetValue(apvts.getRawParameterValue("Amp")->load());

            auto channelDataL = buffer.getWritePointer (0);
            auto channelDataR = buffer.getWritePointer (1);
            auto outchannelDataL = buffer.getWritePointer (0);
            auto outchannelDataR = buffer.getWritePointer (1);
            for(int i = 0; i < buffer.getNumSamples(); ++i)
            {
            
                float offset = 1.f  - dryWetSmooth.getNextValue();
                outchannelDataL[i] =channelDataL[i] * offset + ((detunerOneL->process(channelDataL[i], detuneFreq) + detunerTwoL->process(channelDataL[i], detuneFreq + 0.001f) +  detunerThreeL->process(channelDataL[i], detuneFreq + 0.002f) + detunerFourL->process(channelDataL[i], detuneFreq + 0.003f)) * ampSmooth.getNextValue()) * dryWetSmooth.getNextValue();
                outchannelDataR[i] =channelDataR[i] * offset + ((detunerOneR->process(channelDataR[i], detuneFreq) + detunerTwoR->process(channelDataR[i], detuneFreq + 0.001f) +  detunerThreeR->process(channelDataR[i], detuneFreq + 0.002f) + detunerFourR->process(channelDataR[i], detuneFreq + 0.003f)) * ampSmooth.getNextValue()) * dryWetSmooth.getNextValue();
            }
        
            
        
        
  
    

}

//==============================================================================
bool PitchyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PitchyAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);//PitchyAudioProcessorEditor (*this);
    
}

//==============================================================================
void PitchyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void PitchyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
                if (tree.isValid())
                {
                    apvts.replaceState(tree);
                }
}

juce::AudioProcessorValueTreeState::ParameterLayout PitchyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Detune","Detune",0.01f,1.f,0.5));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Amp","Amp",0.01f,1.f,0.25));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Wet","Wet",0.00f,1.f,0.5));
    
    return layout;
}


void PitchyAudioProcessor::updateFreq()
{
    interFreq = targetFreq / mSampleRate;
}

void PitchyAudioProcessor::updateAmp()
{
    interAmp = targetAmp / mSampleRate;
}

void PitchyAudioProcessor::updateWet()
{
    interWet = targetWet / mSampleRate;
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchyAudioProcessor();
}
