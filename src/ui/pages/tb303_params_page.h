#pragma once

#include "../ui_core.h"
#include "../ui_colors.h"
#include "../ui_utils.h"

class Synth303ParamsPage : public IPage {
 public:
  Synth303ParamsPage(IGfx& gfx, MiniAcid& mini_acid, AudioGuard& audio_guard, int voice_index);
  void draw(IGfx& gfx, int x, int y, int w, int h) override;
  bool handleEvent(UIEvent& ui_event) override;
  const std::string & getTitle() const override;

 private:
  void withAudioGuard(const std::function<void()>& fn);

  IGfx& gfx_;
  MiniAcid& mini_acid_;
  AudioGuard& audio_guard_;
  int voice_index_;
};
