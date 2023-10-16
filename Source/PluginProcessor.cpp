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
                       ),treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    m_Shift = treeState.getRawParameterValue("Shift");
    //m_Noise  = treeState.getRawParameterValue("NoiseLevel");
    m_Wet  = treeState.getRawParameterValue("Mix");
    sTFT = std::make_unique<STFT>();
    wet = std::make_unique<Gain_Block_ST>();
    dry = std::make_unique<Gain_Block_ST>();
}

PitchyAudioProcessor::~PitchyAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
PitchyAudioProcessor::createParameterLayout()
{
    // create parameters
    // you could also use a array with strings and add them in a for loop
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    auto shiftFactor = std::make_unique<juce::AudioParameterFloat>("Shift","Shift",0.1,10.0,5);    
    
   // auto noise = std::make_unique<juce::AudioParameterFloat>("NoiseLevel","NoiseLevel",0.0,1.0,0.5);
    
    auto Mix = std::make_unique<juce::AudioParameterFloat>("Mix","Mix",0.0,1.0,0.5);
    
    params.push_back(std::move(shiftFactor));
    
    params.push_back(std::move(noise));
    params.push_back(std::move(Mix));
    return {params.begin(),params.end()};
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
    sTFT->setup(2);
    sTFT->updateParameters(4096,
                            4,
                           2);
    wet->prepare();
    dry->prepare();
    dry_Buf.setSize(2,samplesPerBlock);
    wet_Buf.setSize(2,samplesPerBlock);
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.

    
    const float* in_L = buffer.getReadPointer(0);
    const float* in_R = buffer.getReadPointer(1);
    
    dry_Buf.copyFrom(0, 0, in_L, buffer.getNumSamples());
    dry_Buf.copyFrom(1, 0, in_R, buffer.getNumSamples());
    wet_Buf.copyFrom(0, 0, in_L, buffer.getNumSamples());
    wet_Buf.copyFrom(1, 0, in_R, buffer.getNumSamples());
    sTFT->getScale(*m_Shift);
    sTFT->processBlock(wet_Buf);
    dry->setGain(1.0f - *m_Wet);
    dry->process(dry_Buf, buffer.getNumSamples());
    
     float* read_ptrLD = dry_Buf.getWritePointer(0);
     float* read_ptrRD = dry_Buf.getWritePointer(1);
    
    wet->setGain(*m_Wet);
    wet->process(wet_Buf, buffer.getNumSamples());
    
     float* read_ptrLW = wet_Buf.getWritePointer(0);
     float* read_ptrRW = wet_Buf.getWritePointer(1);
    
    float* wet_ptrLW = buffer.getWritePointer(0);
    float* wet_ptrRW = buffer.getWritePointer(1);
    
    
    
    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
        
        wet_ptrLW[i] = read_ptrLD[i] + read_ptrLW[i];
        wet_ptrRW[i] = read_ptrRD[i] + read_ptrRW[i];
    }
}

//==============================================================================
bool PitchyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PitchyAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);//new PitchyAudioProcessorEditor (*this);
}

//==============================================================================
void PitchyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PitchyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchyAudioProcessor();
}
