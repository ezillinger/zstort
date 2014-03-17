#include "IControl.h"
#include "AudioProcessor.h"

#include <float.h>
//TODO: destructors, better inheritance

void AudioProcessor::setSampleRate(int sampleRate){
	this->sampleRate = sampleRate;
	reset();
}

int AudioProcessor::getSampleRate(){ return this->sampleRate; }
void AudioProcessor::process(double * inputValue){}
void AudioProcessor::reset(){}
AudioProcessor::AudioProcessor(){ sampleRate = 44100; }

DistortionProcessor::DistortionProcessor(double level){
	this->level = level;
}

void AudioProcessor::enable(){ enabled = true; }
void AudioProcessor::disable(){ enabled = false; }
AudioProcessor::~AudioProcessor(){ }

void DistortionProcessor::process(double * inputValue){
	if (enabled){
		if (*inputValue > level){
			*inputValue = level;
		}
		else if (*inputValue < -level){
			*inputValue = -level;
		}

		*inputValue /= level;
	}
}

void DistortionProcessor::setLevel(double level){
	this->level = level;
}

double DistortionProcessor::getLevel(){
	return this->level;
}

BitCrushProcessor::BitCrushProcessor(int bits){
	this->bits = bits;
	steps = (2 << (bits)-1);
	stepSize = fmax(DBL_MIN, (1. / (double)steps));
}

void BitCrushProcessor::process(double * inputValue){
	if (enabled){
		if (bits < 32){
			*inputValue = ((floor(*inputValue / stepSize)) * stepSize);
		}
	}
}

void BitCrushProcessor::setBits(int bits){
	this->bits = bits;
	steps = (2 << (bits)-1);
	stepSize = fmax(DBL_MIN, (1. / (double)steps));
}

int BitCrushProcessor::getBits(){
	return this->bits;
}

void SampleRateReductionProcessor::process(double * inputValue){
	//apply sampleRate reduction
	if (samplesHeld % ratio != 0){
		*inputValue = sampleHeld;
	}
	else{
		sampleHeld = *inputValue;
	}
}

void SampleRateReductionProcessor::setRatio(int ratio){
	this->ratio = ratio;
}

int SampleRateReductionProcessor::getRatio(){
	return this->ratio;
}

SampleRateReductionProcessor::SampleRateReductionProcessor(int ratio){ this->ratio = ratio; }

void TremoloProcessor::setDepth(double depth){ this->depth = depth; }
double TremoloProcessor::getDepth(){ return this->depth; }
void TremoloProcessor::setRate(double rate){ this->rate = rate; osc->SetFreq(rate); }
double TremoloProcessor::getRate(){ return this->rate; }
void TremoloProcessor::setSamplesPerPhase(int spp){ this->samplesPerPhase = spp; sampleCounter = 0; }
int TremoloProcessor::getSamplesPerPhase(){ return this->samplesPerPhase; }

TremoloProcessor::TremoloProcessor(double rate, double depth, int samplesPerPhase, int sampleRate){
	this->sampleRate = sampleRate;
	this->rate = rate;
	this->depth = depth;
	this->samplesPerPhase = samplesPerPhase;
	this->sampleCounter = 0;

	this->osc = new FastSineGenerator(rate, sampleRate);

}

void TremoloProcessor::process(double * inputValue){
	if (!(sampleCounter % samplesPerPhase)){
		sampleCounter = 0;
		lastAmplitude = osc->GetNextSample();
	}
	*inputValue = *inputValue * (0.5*lastAmplitude*(1 - depth) + 0.5 + depth/2);
	sampleCounter++;
}

TremoloProcessor::~TremoloProcessor(){
	delete osc;
}