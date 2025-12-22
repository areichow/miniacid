#pragma once

#include <stdint.h>

#include "mini_dsp_params.h"

enum class DrumParamId : uint8_t {
  MainVolume = 0,
  Count
};

class DrumSynthVoice {
public:
  explicit DrumSynthVoice(float sampleRate);

  void reset();
  void setSampleRate(float sampleRate);
  void triggerKick();
  void triggerSnare();
  void triggerHat();
  void triggerOpenHat();
  void triggerMidTom();
  void triggerHighTom();
  void triggerRim();
  void triggerClap();

  float processKick();
  float processSnare();
  float processHat();
  float processOpenHat();
  float processMidTom();
  float processHighTom();
  float processRim();
  float processClap();

  const Parameter& parameter(DrumParamId id) const;
  void setParameter(DrumParamId id, float value);

private:
  float frand();

  float kickPhase;
  float kickFreq;
  float kickEnvAmp;
  float kickEnvPitch;
  bool kickActive;

  float snareEnvAmp;
  float snareToneEnv;
  bool snareActive;
  float snareBp;
  float snareLp;
  float snareTonePhase;
  float snareTonePhase2;

  float hatEnvAmp;
  float hatToneEnv;
  bool hatActive;
  float hatHp;
  float hatPrev;
  float hatPhaseA;
  float hatPhaseB;

  float openHatEnvAmp;
  float openHatToneEnv;
  bool openHatActive;
  float openHatHp;
  float openHatPrev;
  float openHatPhaseA;
  float openHatPhaseB;

  float midTomPhase;
  float midTomEnv;
  bool midTomActive;

  float highTomPhase;
  float highTomEnv;
  bool highTomActive;

  float rimPhase;
  float rimEnv;
  bool rimActive;

  float clapEnv;
  float clapTrans;
  float clapNoise;
  bool clapActive;
  float clapDelay;

  float sampleRate;
  float invSampleRate;

  Parameter params[static_cast<int>(DrumParamId::Count)];
};

