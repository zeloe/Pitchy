/*
  ==============================================================================

    Pitchy.h
    Created: 29 May 2022 8:06:26pm
    Author:  Zeno  Loesch

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "WaveShapes.h"
class Pitchy
{
public:
    Pitchy();
    ~Pitchy();
    
    float process(float inAudio, float frequency);
    double delaylineinter(float inDelayTimeInSamples,  float* inputBuffer, float inputAudio);
    inline float linearinterpol(float v0, float v1, float t);
    float phasor(float frequency);
    float outofphasephasor(float frequency);
    float processWaveShape(float frequency, float index, const double* buffer);
    
    void getTheSampleRate(double sampleRate);
    void getTheBlockSize(int blocksize);
    double InterCubic(float x0, float x1, float x2, float x3, float t);
    double filtering(float inputaudio);
    float mSampleRate = 0;
    float mblocksize = 0;
    int MaxBufferDelaySize = 5000;
    float mDelayBuffer_1 [5000 + 1] {0};
    float mDelayBuffer_2 [5000 + 1] {0};
    std::unique_ptr<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> delayLine1;
    std::unique_ptr<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> delayLine2;
/*
 to do little cleanup
 */
private:
    float phasor_1;
    float outofphasephasor_1;
    int mDelayIndex = 0;
    float phasein = 0;
    float phaseout = 0;
    float delay1 = 0.0f;
    float delay2 = 0.0f;
    float output = 0.0f;
    float g = 0.5f;
    int len = 1;
    int x1 = 0, x2 = 0, x3 = 0, x0 = 0;
    float preout = 0.0f;
    float preout2 = 0.0f;
    float preout3 = 0.0f;
    float preout4 = 0.0f;
    float preout5 = 0.0f;
    float preout6 = 0.0f;
    float preout7 = 0.0f;
    float preout8 = 0.0f;
    float phase1 = 0.0f;
    float phase2 = 0.25f;
    float outputfinal = 0.0f;
    float output2 = 0.0f;
    float outphasor1 = 0.0f;
    float outphasror2 = 0.0f;
    float temp = 0.0f;
    float tempProcess = 0.0f;
};
