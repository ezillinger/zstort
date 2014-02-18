#ifndef __ZSTORT__
#define __ZSTORT__

#include "IPlug_include_in_plug_hdr.h"
#include "sinewavegen.h"
#include "fastsine.h"

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
  double mTremGain;
  double mTremFreq;
  double mDist;
  int mRate;
  int mBits;
  double mstepSizeBC;
  FastSineGenerator * zWave;
  
};

#endif
