

#pragma once
/*
  ==============================================================================

    Sine_Wave.h
    Created: 17 Jan 2023 5:37:34pm
    Author:  Onez

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"
class Gain_Block
{
public:
    Gain_Block()
    {
    };
    ~Gain_Block() {};
    
    void prepare()
    {
        temp_gain = 0;
        
    }
    
    void setGain(float gain)
    {
        current_gain = gain;
    }
    
    void process(float* inputptr, const int numSamples) noexcept
    {
        //Mono
        auto* input  = inputptr;
        auto* output = inputptr;
         
        if(temp_gain != current_gain)
        {
            // this works for block based processing
            gain_inc = (current_gain - temp_gain) / numSamples;
            for (size_t i = 0; i < numSamples; ++i)
            {
                temp_gain += gain_inc;
                const float gain = input[i] * temp_gain;
                output[i] = gain;
            }
            temp_gain = current_gain;
        } else {
            for (size_t i = 0; i < numSamples; ++i)
            {
                const float gain = input[i] * temp_gain;
                output[i] = gain;
            }
        }
             
    }
private:
    float temp_gain;
    float current_gain;
    float gain_inc = 0;
    int numSamples = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gain_Block)
};
class Gain_Block_ST
{
public:
    Gain_Block_ST()
    {
    };
    ~Gain_Block_ST() {};
    
    void prepare()
    {
        temp_gain = 0;
    }
    
    void setGain(float gain)
    {
        current_gain = gain;
    }
    
    void process(juce::AudioBuffer<float>& buf, const int numSamples) noexcept
    {
        //Stereo
         float * inputL  = buf.getWritePointer(0);
        float* outputL = buf.getWritePointer(0);
        
         float* inputR  = buf.getWritePointer(1);
        float* outputR = buf.getWritePointer(1);
         
        if(temp_gain != current_gain)
        {
            // this works for block based processing
            gain_inc = (current_gain - temp_gain) / numSamples;
            for (size_t i = 0; i < numSamples; ++i)
            {
                temp_gain += gain_inc;
                
                outputL[i] = inputL[i] * temp_gain;
                outputR[i] = inputR[i] * temp_gain;
            }
            temp_gain = current_gain;
        } else {
            for (size_t i = 0; i < numSamples; ++i)
            {
                
                
                outputL[i] = inputL[i] * temp_gain;
                outputR[i] = inputR[i] * temp_gain;
            }
        }
             
    }
private:
    float temp_gain;
    float current_gain;
    float gain_inc = 0;
    int numSamples = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gain_Block_ST)
};
