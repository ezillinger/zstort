#ifndef _AUDIO_PROCESSOR_H_
#define _AUDIO_PROCESSOR_H_

#include "IControl.h"
#include "fastsine.h"
#include <math.h>

class AudioProcessor
{

protected:
	int sampleRate;
	bool enabled;

public:

	virtual void setSampleRate(int sampleRate);
	int getSampleRate();
	virtual void process(double * inputValue);
	virtual void reset();
	virtual void enable();
	virtual void disable();
	AudioProcessor();

};

class DistortionProcessor : public AudioProcessor{
protected:
	double level;
public:	
	DistortionProcessor(double level);
	void process(double * inputValue);
	void setLevel(double level);
	double getLevel();
		
};

class BitCrushProcessor : public AudioProcessor{
protected:
	double bits;
	unsigned long int steps;
	double stepSize;
public:
	BitCrushProcessor(int bits);
	void process(double * inputValue);
	void setBits(int bits);
	int getBits();

};

class SampleRateReductionProcessor : public AudioProcessor{
protected:
	int ratio;
	int samplesHeld;
	double sampleHeld;
public:
	virtual void process(double * inputValue);
	int SampleRateReductionProcessor::getRatio();
	void SampleRateReductionProcessor::setRatio(int ratio);
	SampleRateReductionProcessor::SampleRateReductionProcessor(int ratio);

};

class TremoloProcessor : public AudioProcessor{
protected:
	int samplesPerPhase;
	double rate;
	double depth;
	FastSineGenerator * osc;

public:
	virtual void process(double * inputValue);
	double getRate();
	void setRate(double rate);
	double getDepth();
	void setDepth(double depth);
	void setSamplesPerPhase(int spf);
	int getSamplesPerPhase();

	
	TremoloProcessor(double rate, double depth, int samplesPerPhase = 1);

};
#endif