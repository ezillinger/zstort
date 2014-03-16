#ifndef _FASTSINE_H_
#define _FASTSINE_H_



//wavetable sine generator

class FastSineGenerator
{
	double freq;
	double sampleRate;
	int phase = 0;

public:
	//constructors
	FastSineGenerator(double freq, int sampleRate = 44100) { this->freq = freq; this->sampleRate = sampleRate; }
	FastSineGenerator() { this->freq = 0; this->sampleRate = 44100; }
	//destructor
	~FastSineGenerator() { }
	//set frequency
	void SetFreq(double freq)
	{
		phase = 0;
		this->freq = freq;

	}
	//get frequency
	double GetFreq(){
		return freq;
	}
	// returns sine
	double GetNextSample() 
	{
		if (phase > sampleRate / freq){
			phase = 0;
		}
		
		//double poop = 2. * PI * phase * freq / (sampleRate);
		
		return sin(2. * PI * phase++ * freq / (sampleRate));
		
	}
	// returns cos
	double GetNextCos()  // call BEFORE Gen() if you want the cosine of the next value
	{
		return 0.;
	}

};

#endif
