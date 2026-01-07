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

  // Triggers
  void triggerKick();
  void triggerSnare();
  void triggerHat();
  void triggerOpenHat();
  void triggerMidTom();
  void triggerHighTom();
  void triggerRim();
  void triggerClap();

  // Processors (one sample per call)
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

  // --- Kick (808-style) ---
  float kickPhase;
  float kickEnvAmp;
  float kickEnvPitch;
  bool  kickActive;

  // --- Snare ---
  float snareEnvAmp;
  float snareToneEnv;
  bool  snareActive;
  float snareBp;
  float snareLp;
  float snareTonePhase;
  float snareTonePhase2;

  // --- Hats (606-style source + robust biquad BPF) ---
  float hatEnvAmp;
  float hatToneEnv;
  bool  hatActive;
  float hatPhases[6];     // six square oscillators
  float hatOscFreqs[6];   // base oscillators' frequencies
  // Biquad band-pass states (~7.1 kHz)
  float hatBP_x1, hatBP_x2, hatBP_y1, hatBP_y2;
  float openHatBP_x1, openHatBP_x2, openHatBP_y1, openHatBP_y2;
  // Biquad coefficients
  float bp_b0, bp_b1, bp_b2, bp_a1, bp_a2;
  // Post-VCA highpass
  float hatHP_y1, hatHP_x1;
  float openHatHP_y1, openHatHP_x1;

  float openHatEnvAmp;
  float openHatToneEnv;
  bool  openHatActive;
  float openHatPhases[6];

  // --- Toms ---
  float midTomPhase;
  float midTomEnv;
  bool  midTomActive;
  float highTomPhase;
  float highTomEnv;
  bool  highTomActive;

  // --- Rim ---
  float rimPhase;
  float rimEnv;
  bool  rimActive;

  // --- Clap ---
  float clapEnv;
  float clapTrans;
  float clapNoise;
  bool  clapActive;
  float clapDelay;

  // --- System ---
  float sampleRate;
  float invSampleRate;

  // Parameters
  Parameter params[static_cast<int>(DrumParamId::Count)];
};
