#include "waveform_page.h"

WaveformPage::WaveformPage(IGfx& gfx, MiniAcid& mini_acid, AudioGuard& audio_guard)
  : gfx_(gfx),
    mini_acid_(mini_acid),
    audio_guard_(audio_guard),
    wave_color_index_(0)
{
}

void WaveformPage::draw(IGfx& gfx, int x, int y, int w, int h) {
  int wave_y = y + 2;
  int wave_h = h - 2;
  if (w < 4 || wave_h < 4) return;

  int16_t samples[AUDIO_BUFFER_SAMPLES/2];
  size_t sampleCount = mini_acid_.copyLastAudio(samples, AUDIO_BUFFER_SAMPLES/2);
  int mid_y = wave_y + wave_h / 2;

  gfx_.setTextColor(IGfxColor::Orange());
  gfx_.drawLine(x, mid_y, x + w - 1, mid_y);

  if (sampleCount > 1) {
    IGfxColor waveColor = WAVE_COLORS[wave_color_index_ % NUM_WAVE_COLORS];
    int amplitude = wave_h / 2 - 2;
    if (amplitude < 1) amplitude = 1;
    for (int px = 0; px < w - 1; ++px) {
      size_t idx0 = (size_t)((uint64_t)px * sampleCount / w);
      size_t idx1 = (size_t)((uint64_t)(px + 1) * sampleCount / w);
      if (idx0 >= sampleCount) idx0 = sampleCount - 1;
      if (idx1 >= sampleCount) idx1 = sampleCount - 1;
      float s0 = samples[idx0] / 32768.0f;
      float s1 = samples[idx1] / 32768.0f;
      int y0 = mid_y - static_cast<int>(s0 * amplitude);
      int y1 = mid_y - static_cast<int>(s1 * amplitude);
      drawLineColored(gfx_, x + px, y0, x + px + 1, y1, waveColor);
    }
  } 
}

bool WaveformPage::handleEvent(UIEvent& ui_event) {
  if (ui_event.event_type != MINIACID_KEY_DOWN) return false;
  switch (ui_event.scancode) {
    case MINIACID_UP:
    case MINIACID_DOWN:
      wave_color_index_ = (wave_color_index_ + 1) % NUM_WAVE_COLORS;
      return true;
    default:
      break;
  }
  return false;
}

const std::string & WaveformPage::getTitle() const {
  static std::string title = "WAVEFORM";
  return title;
}

