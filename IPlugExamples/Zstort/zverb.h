#ifndef _ZVERB_H_
#define _ZVERB_H_

class ZVerb
{
	double delaySeconds;
	int sampleRate;
	double wetDry;
	double decay = 0.5;
	int delaySamples;

	double * delayBuffer;
	enum reverbMode {
		simple = 0,
		enhanced,
		numModes
	};
	reverbMode mode;

	double reset(){

	}

public:
	//constructor
	ZVerb(double delay = 500, double wetDry = 0.5, reverbMode mode = simple){
		this->delaySeconds = delay;
		this->wetDry = wetDry;
		this->mode = mode;

		this->delaySamples = delaySeconds * sampleRate;
	}
	
	//destructor
	~ZVerb() {
		
	}
	//set frequency
	void setSampleRate(int sampleRate){
		this->sampleRate = sampleRate;
	}
	//set delay
	void setDelay(double delay){
		this->delaySeconds = delay;
	}
	//set wet/dry
	void setWetDry(double wetDry){
		this->wetDry = wetDry;
	}
	//get next sample
	double process(double * inputValue){

		switch (mode){
		case simple:
			for (int i = 0; i < delaySamples; i++){

			}
			break;
		case enhanced:

			break;

		}

	}

	

};

#endif