/*
  ==============================================================================
    Pitchy.cpp
    Created: 29 May 2022 8:06:26pm
    Author:  Zeno  Loesch
  ==============================================================================
*/

#include "Pitchy.h"

Pitchy::Pitchy():
phasor_1(0.0f),
outofphasephasor_1(0.25f),
phasor_2(0.0f),
outofphasephasor_2(0.25f)
{
    delayLine1 = std::make_unique<juce::dsp::DelayLine<float,juce::dsp::DelayLineInterpolationTypes::Linear>> (5000);
    delayLine2 = std::make_unique<juce::dsp::DelayLine<float,juce::dsp::DelayLineInterpolationTypes::Linear>> (5000);
}

Pitchy::~Pitchy()
{
    
    
}

float Pitchy::phasor(float frequency)
{
    //maximilian.h
    phasor_1 += (1./(mSampleRate/(frequency)));
    if (phasor_1 > 1.0) phasor_1 -= 1.f ;
    return phasor_1;
    
}

float Pitchy::outofphasephasor(float frequency)
{
    //maximilian.h
    outofphasephasor_1 += (1./(mSampleRate/(frequency)));
    if (outofphasephasor_1 > 1.0) outofphasephasor_1 -= 1.f;
    return outofphasephasor_1;
    
}

float Pitchy::phasor2(float frequency)
{
    //maximilian.h
    phasor_2 += (1./(mSampleRate/(frequency)));
    if (phasor_2 > 1.0) phasor_2 -= 1.f ;
    return phasor_2;
    
}

float Pitchy::outofphasephasor2(float frequency)
{
    //maximilian.h
    outofphasephasor_2 += (1./(mSampleRate/(frequency)));
    if (outofphasephasor_2 > 1.0) outofphasephasor_2 -= 1.f;
    return outofphasephasor_2;
    
}
float Pitchy::cubic_hermite_interpolation(float frequency, float index, const double *buffer)
{
    //maximilian.h
    //cubic hermite interpolation
    double remainder;
        double a,b,c,d,a1,a2,a3;
        double output;
        index = index * 512.f;
        if ( index > 512 ) index -=512;
        remainder = index - floor(index);

        if (index==0) {
            a=buffer[(long) 512];
            b=buffer[(long) index];
            c=buffer[(long) index+1];
            d=buffer[(long) index+2];

        } else {
            a=buffer[(long) index-1];
            b=buffer[(long) index];
            c=buffer[(long) index+1];
            d=buffer[(long) index+2];

        }

        a1 = 0.5f * (c - a);
        a2 = a - 2.5 * b + 2.f * c - 0.5f * d;
        a3 = 0.5f * (d - a) + 1.5f * (b - c);
        output = double (((a3 * remainder + a2) * remainder + a1) * remainder + b);
        return(output);
}

    
    
    



inline float Pitchy::linearinterpol(float  v0, float  v1, float  t )
{
    //wikipedia
    return (1.f - (t)) * v0 + (t) * v1;
};


double Pitchy::InterCubic(float x0, float x1, float x2, float x3, float t)
{
    //from stackoverflow
    float c0 = x1;
    float c1 = .5f * (x2 - x0);
    float c2 = x0 - (2.5f * x1) + (2 * x2) - (.5f * x3);
    float c3 = (.5f * (x3 - x0)) + (1.5f * (x1 - x2));
    return (((((c3 * t) + c2) * t) + c1) * t) + c0;
}


double Pitchy::delaylineinter(float inDelayTimeInSamples, float *inputBuffer, float inputAudio)
{
   
    //working delay
   
     mDelayIndex = (++mDelayIndex ) % MaxBufferDelaySize;
     
     inputBuffer[mDelayIndex] = inputAudio;
     
    double readPosition = (double)(mDelayIndex) - inDelayTimeInSamples;
                       
                        if (readPosition < 0.0f) {
                            readPosition = readPosition + MaxBufferDelaySize;
                        }
                       
                       
                        int index_y0 = (int)readPosition - 1;
                        if(index_y0 <= 0){
                            index_y0 = index_y0 + MaxBufferDelaySize;
                        }
                         
                        int index_y1 = readPosition;
                        
                        if(index_y1 > MaxBufferDelaySize){
                            index_y1 = index_y1 - MaxBufferDelaySize;
                        }
                        
                        const float sample_y0 = inputBuffer[index_y0];
                        const float sample_y1 = inputBuffer[index_y1];
                        const float t = readPosition - (int)readPosition;
                        
    output2 = ((linearinterpol(sample_y0, sample_y1, t)));
    return output2;
    
    /*
     working delay but not changing pitch
    mDelayIndex = (++mDelayIndex ) % MaxBufferDelaySize;
    
    inputBuffer[mDelayIndex] = inputAudio;
    
    auto newRead = float(mDelayIndex) + std::max (inDelayTimeInSamples, float(MaxBufferDelaySize));
    auto read = std::fmod (newRead, float(MaxBufferDelaySize));
    double readPosition_1 = read - std::floorf(read);
    auto frac = readPosition_1 - std::floorf(readPosition_1);
      auto x0 = int( std::floorf(read));
      auto x1 = (x0 + 1) % MaxBufferDelaySize;
      auto y0 = inputBuffer[x0];
      auto y1 = inputBuffer[x1];
      inputBuffer[mDelayIndex] = inputAudio;
      mDelayIndex = (mDelayIndex + 1) % MaxBufferDelaySize;
      read = std::fmod (read + 1.0, float(MaxBufferDelaySize));
      
      const double output2 = (1.0 - frac) * y0 + frac * y1;
      
     */
     
     return output2;
     
    
    
}

float Pitchy::process(float inAudio, float frequency, float offsetamp, float offsetfreq)
{
    
    phasein = this->phasor(frequency);
    phasein2 = this->phasor2(frequency * offsetfreq);
    phaseout = this->outofphasephasor(frequency);
    phaseout2 = this->outofphasephasor2(frequency * offsetfreq);
    
    //if (mDelayIndex > MaxBufferDelaySize) mDelayIndex -= mDelayIndex;
    delay1 = this->delaylineinter(phasein2, mDelayBuffer_1, inAudio);
    delay2 = this->delaylineinter(phaseout2 , mDelayBuffer_2, inAudio);
    tempProcess = delay1 * (this->cubic_hermite_interpolation(frequency  * offsetfreq, phasein2, possineBuffer)) + delay2 * (this->cubic_hermite_interpolation(frequency * offsetfreq, phaseout2, possineBuffer)) * -1.f;
    delayLine1->pushSample(0, inAudio);
    delayLine1->setDelay(phasein  * mSampleRate * 0.1f);
    delayLine2->pushSample(0, inAudio);
    delayLine2->setDelay(phaseout * mSampleRate * 0.1f);
    output = (delayLine1->popSample(0) * (this->cubic_hermite_interpolation(frequency + offsetfreq, phasein, possineBuffer)) + delayLine2->popSample(0) * (this->cubic_hermite_interpolation(frequency + offsetfreq, phaseout, possineBuffer)) * -1.f) + tempProcess * offsetamp;
return output;
}

 
void Pitchy::getTheSampleRate(double sampleRate)
{
    mSampleRate = sampleRate;
}

void Pitchy::getTheBlockSize(int blocksize)
{
    mblocksize = static_cast<float>(blocksize);
}
