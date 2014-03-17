#ifndef __ZSTORT__
#define __ZSTORT__

#include "IPlug_include_in_plug_hdr.h"
#include "sinewavegen.h"
#include "fastsine.h"
#include "AudioProcessor.h"

class Zstort : public IPlug
{
public:
  Zstort(IPlugInstanceInfo instanceInfo);
  ~Zstort();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
  DistortionProcessor * distortion;
  BitCrushProcessor * bitCrusher;
  SampleRateReductionProcessor * rateReducer;
  TremoloProcessor * trem;

  int distIdx;
  
};

#endif
