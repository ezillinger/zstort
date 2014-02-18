#include "Zstort.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "sinewavegen.h"
#include "fastsine.h"
#include "zverb.h"


const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kTremGain,
  kTremFreq,
  kDist,
  kBits,
  kRate,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 50,
  kGainY = 50,

  kTremGainX = 100,
  kTremGainY = 50,

  kTremFreqX = 50,
  kTremFreqY = 100,

  kDistX = 100,
  kDistY = 100,

  kBitsX = 150,
  kBitsY = 50,

  kRateX = 150,
  kRateY = 100,

  kKnobFrames = 60
};

Zstort::Zstort(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 8., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(2.);

  GetParam(kTremGain)->InitDouble("TremDepth", 100., 0., 100.0, 0.01, "%");
  GetParam(kTremGain)->SetShape(2.);

  GetParam(kTremFreq)->InitDouble("TremFreq", 4., 0.01, 15, 0.01, "hz");
  //GetParam(kTremFreq)->SetShape(2.);
  
  GetParam(kDist)->InitDouble("Distortion", 100., 0.1, 100., 0.01, "%");
  GetParam(kDist)->SetShape(2.);

  GetParam(kBits)->InitInt("BitRate", 32, 1, 32, "bits");

  GetParam(kRate)->InitInt("RateReduction", 1, 1, 32, "x");


  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);
 

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kTremGainX, kTremGainY, kTremGain, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kTremFreqX, kTremFreqY, kTremFreq, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDistX, kDistY, kDist, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kBitsX, kBitsY, kBits, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kRateX, kRateY, kRate, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  this->zWave = new FastSineGenerator(440.);

}

Zstort::~Zstort() {}

void Zstort::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	double temp1;
	double temp2;

	double last1;
	double last2;

	double tremTemp;

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
	{
		//apply trem

		tremTemp = zWave->GetNextSample();
		temp1 = *in1 * fmin(0.5*(tremTemp + 1.) + mTremGain, 1);
		temp2 = *in2 * fmin(0.5*(tremTemp + 1.) + mTremGain, 1);

		//apply distortion

		if (temp1 >= 0){
			temp1 = fmin(temp1, mDist);
		}
		else{
			temp1 = fmax(temp1, -mDist);
		}

		if (temp2 >= 0){
			temp2 = fmin(temp2, mDist);
		}
		else{
			temp2 = fmax(temp2, -mDist);
		}

		temp1 /= mDist;
		temp2 /= mDist;

		//DBGMSG("output = %d\n", temp1 * mGain);

		//apply bitcrusher

		if (mBits < 32){
		temp1 = ((floor(temp1 / mstepSizeBC)) * mstepSizeBC);
		temp2 = ((floor(temp2 / mstepSizeBC)) * mstepSizeBC);
		}

		//apply sampleRate reduction
		if (s % mRate != 0){
			*out1 = last1 * mGain;
			*out2 = last2 * mGain;
		}
		else{
			last1 = temp1;
			last2 = temp2;
			//apply gain
			*out1 = temp1 * mGain;
			*out2 = temp2 * mGain;
		}



	}
}

void Zstort::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void Zstort::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  unsigned long int big;
  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 10;
      break;

	case kTremGain:
		mTremGain = GetParam(kTremGain)->Value() / 100;
		break;

	case kTremFreq:
		mTremFreq = GetParam(kTremFreq)->Value();
		zWave->SetFreq(mTremFreq);
		break;

	case kDist:
		mDist = GetParam(kDist)->Value() / 100.;
		break;

	case kBits:
		mBits = GetParam(kBits)->Int();
		big = (2 << (mBits) -  1);
		mstepSizeBC = fmax(DBL_MIN, (1. / (double)big));
		DBGMSG("size: %i", mBits);
		break;
	case kRate:
		mRate = GetParam(kRate)->Int();
		break;

    default:
      break;
  }
}
