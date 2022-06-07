/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Pitchy.h"
//==============================================================================
/**
*/

inline float pfreq (float factor, float dw)
{
    return(factor * 1000.f/dw + 1.f);
}

class PitchyAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PitchyAudioProcessor();
    ~PitchyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    void initDsp()
        {
               
                    detunerOneL = std::make_unique<Pitchy>();
                
                    detunerTwoL = std::make_unique<Pitchy>();
                
                    detunerThreeL = std::make_unique<Pitchy>();
                    
                    detunerFourL = std::make_unique<Pitchy>();
        
                    detunerOneR = std::make_unique<Pitchy>();
    
                    detunerTwoR = std::make_unique<Pitchy>();
    
                    detunerThreeR = std::make_unique<Pitchy>();
        
                    detunerFourR = std::make_unique<Pitchy>();
              
        }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};
private:
    std::unique_ptr<Pitchy> detunerOneL;
    std::unique_ptr<Pitchy> detunerTwoL;
    std::unique_ptr<Pitchy> detunerThreeL;
    std::unique_ptr<Pitchy> detunerFourL;
    std::unique_ptr<Pitchy> detunerOneR;
    std::unique_ptr<Pitchy> detunerTwoR;
    std::unique_ptr<Pitchy> detunerThreeR;
    std::unique_ptr<Pitchy> detunerFourR;
    juce::LinearSmoothedValue<float> detuneSmooth {0.0f};
    juce::LinearSmoothedValue<float> dryWetSmooth {0.0f};
    juce::LinearSmoothedValue<float> ampSmooth {0.0f};
    juce::LinearSmoothedValue<float> offsetSmooth {0.0f};
    juce::LinearSmoothedValue<float> ampTuneSmooth {0.0f};
    juce::LinearSmoothedValue<float> ampTuneAmountSmooth {0.0f};
    //float freq = 0.0f;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchyAudioProcessor)
};
