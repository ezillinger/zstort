#include "Zstort.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "sinewavegen.h"
#include "fastsine.h"
#include "zverb.h"
#include "AudioProcessor.h"


const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kTremDepth,
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

  kGainX = 300,
  kGainY = 200,

  kTremDepthX = 100,
  kTremDepthY = 100,

  kTremFreqX = 200,
  kTremFreqY = 100,

  kDistX = 300,
  kDistY = 100,

  kBitsX = 100,
  kBitsY = 200,

  kRateX = 200,
  kRateY = 200,

  kKnobFrames = 60
};

Zstort::Zstort(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 8., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(2.);

  GetParam(kTremDepth)->InitDouble("TremDepth", 100., 0., 100.0, 0.01, "%");
  GetParam(kTremDepth)->SetShape(2.);

  GetParam(kTremFreq)->InitDouble("TremFreq", 4., 0.01, 15, 0.01, "hz");
  //GetParam(kTremFreq)->SetShape(2.);
  
  GetParam(kDist)->InitDouble("Distortion", 100., 0.1, 100., 0.01, "%");
  GetParam(kDist)->SetShape(2.);

  GetParam(kBits)->InitInt("BitRate", 32, 1, 32, "bits");

  GetParam(kRate)->InitInt("RateReduction", 1, 1, 32, "x");


  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_BLACK);
 
  IRECT distRect(kDistX, kDistY - 5, 200, 80.);
  IText textProps3(14, &COLOR_RED, "Arial", IText::kStyleItalic, IText::kAlignNear, 0, IText::kQualityDefault);
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDistX, kDistY -25, 200, 80), &textProps3, "Distortion"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kGainX, kGainY - 25, 200, 80), &textProps3, "Gain"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kTremDepthX, kTremDepthY - 25, 200, 80), &textProps3, "Tremolo Depth"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kTremFreqX, kTremFreqY - 25, 200, 80), &textProps3, "Tremolo\nFrequency"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kRateX, kRateY - 25, 200, 80), &textProps3, "Sample Rate\nReduction"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kBitsX, kBitsY - 25, 200, 80), &textProps3, "BitCrusher"));
  
  //attempt at updating values live -can't seem to redraw
  distIdx = pGraphics->AttachControl(new ITextControl(this, IRECT(200, 300, 200, 80), &textProps3, "Distortion: xx!"));


  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kTremDepthX, kTremDepthY, kTremDepth, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kTremFreqX, kTremFreqY, kTremFreq, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDistX, kDistY, kDist, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kBitsX, kBitsY, kBits, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kRateX, kRateY, kRate, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  this->distortion = new DistortionProcessor(1.);
  this->bitCrusher = new BitCrushProcessor(32);
  this->rateReducer = new SampleRateReductionProcessor(1);
  this->trem = new TremoloProcessor(4., 1, 2, GetSampleRate());
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
		//apply tremolo

		temp1 = *in1;
		temp2 = *in2;

		trem->process(&temp1);
		trem->process(&temp2);

		//apply distortion

		distortion->process(&temp1);
		distortion->process(&temp2);

		//apply bitcrusher

		bitCrusher->process(&temp1);
		bitCrusher->process(&temp2);

		//apply sample rate reduction

		rateReducer->process(&temp1);
		rateReducer->process(&temp2);

		//apply gain

		*out1 = temp1 * mGain;
		*out2 = temp2 * mGain;

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

  ITextControl * tP;
  char charBuffer[100];
  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 10;
      break;

	case kTremDepth:
		trem->setDepth(GetParam(kTremDepth)->Value() / 100);
		break;

	case kTremFreq:
		trem->setRate(GetParam(kTremFreq)->Value());
		break;

	case kDist:
		//doesn't redraw correctly, scroll mousewheel on text to update
		distortion->setLevel(GetParam(kDist)->Value() / 100.);
		tP = (ITextControl *) this->GetGUI()->GetControl(distIdx);
		sprintf(charBuffer, "Distortion: %f", distortion->getLevel());
		DBGMSG(charBuffer);
		tP->SetTextFromPlug(charBuffer);
		tP->SetDirty(); 
		tP->Redraw();
		break;
	case kBits:
		bitCrusher->setBits(GetParam(kBits)->Int());
		break;
	case kRate:
		rateReducer->setRatio(GetParam(kRate)->Int());
		break;

    default:
      break;
  }
}
