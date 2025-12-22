#include "help_page.h"

#include <cstdio>

HelpPage::HelpPage()
  : help_page_index_(0),
    total_help_pages_(6) {}

void HelpPage::drawHelpPage(IGfx& gfx,int x, int y, int w, int h, int helpPage) {
  int body_y = y;
  int body_h = h;
  if (body_h <= 0) return;

  int col_w = w / 2 - 6;
  int left_x = x + 4;
  int right_x = x + col_w + 10;
  int left_y = body_y + 4;
  int lh = 10;
  int right_y = body_y + 4 + lh;

  auto heading = [&](int px, int py, const char* text) {
    gfx.setTextColor(COLOR_ACCENT);
    gfx.drawText(px, py, text);
    gfx.setTextColor(COLOR_WHITE);
  };
  auto item = [&](int px, int py, const char* key, const char* desc, IGfxColor keyColor) {
    gfx.setTextColor(keyColor);
    gfx.drawText(px, py, key);
    gfx.setTextColor(COLOR_WHITE);
    int key_w = textWidth(gfx, key);
    gfx.drawText(px + key_w + 6, py, desc);
  };

  if (helpPage == 0) {
    heading(left_x, left_y, "Transport");
    left_y += lh;
    item(left_x, left_y, "SPACE", "play/stop", IGfxColor::Green());
    left_y += lh;
    item(left_x, left_y, "K / L", "BPM -/+", IGfxColor::Cyan());
    left_y += lh;

    heading(left_x, left_y, "Pages");
    left_y += lh;
    item(left_x, left_y, "[ / ]", "prev/next page", COLOR_LABEL);
    left_y += lh;

    heading(left_x, left_y, "Playback");
    left_y += lh;
    item(left_x, left_y, "I / O", "303A/303B randomize", IGfxColor::Yellow());
    left_y += lh;
    item(left_x, left_y, "P", "drum randomize", IGfxColor::Yellow());
    left_y += lh;

  } else if (helpPage == 1) {
    heading(left_x, left_y, "303 (active page voice)");
    left_y += lh;
    item(left_x, left_y, "A / Z", "cutoff + / -", COLOR_KNOB_1);
    left_y += lh;
    item(left_x, left_y, "S / X", "res + / -", COLOR_KNOB_2);
    left_y += lh;
    item(left_x, left_y, "D / C", "env amt + / -", COLOR_KNOB_3);
    left_y += lh;
    item(left_x, left_y, "F / V", "decay + / -", COLOR_KNOB_4);
    left_y += lh;
    item(left_x, left_y, "M", "toggle delay", IGfxColor::Magenta());

    heading(right_x, right_y, "Mutes");
    right_y += lh;
    item(right_x, right_y, "1", "303A", IGfxColor::Orange());
    right_y += lh;
    item(right_x, right_y, "2", "303B", IGfxColor::Orange());
    right_y += lh;
    item(right_x, right_y, "3-0", "Drum Parts", IGfxColor::Orange());
    right_y += lh;
  } else if (helpPage == 2) {
    heading(left_x, left_y, "303 Pattern Edit");
    left_y += lh;
    heading(left_x, left_y, "Navigation");
    left_y += lh;
    item(left_x, left_y, "LEFT/RIGHT", "", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "UP/DOWN", "move", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "ENTER", "Load pattern", IGfxColor::Green());
    left_y += lh;

    heading(left_x, left_y, "Pattern slots");
    left_y += lh;
    item(left_x, left_y, "Q..I", "Pick pattern", COLOR_PATTERN_SELECTED_FILL);
    left_y += lh;

    int ry = body_y + 4 + lh;
    heading(right_x, ry, "Step edits");
    ry += lh;
    item(right_x, ry, "Q", "Toggle slide", COLOR_SLIDE);
    ry += lh;
    item(right_x, ry, "W", "Toggle accent", COLOR_ACCENT);
    ry += lh;
    item(right_x, ry, "A / Z", "Note +1 / -1", COLOR_303_NOTE);
    ry += lh;
    item(right_x, ry, "S / X", "Octave + / -", COLOR_LABEL);
    ry += lh;
    item(right_x, ry, "BACK", "Clear step", IGfxColor::Red());
    ry += lh;
  } else if (helpPage == 3) {
    heading(left_x, left_y, "Drums Pattern Edit");
    left_y += lh;
    heading(left_x, left_y, "Navigation");
    left_y += lh;
    item(left_x, left_y, "LEFT / RIGHT", "", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "UP / DOWN", "move", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "ENTER", "Load/toggle ", IGfxColor::Green());
    left_y += lh;

    heading(left_x, left_y, "Patterns");
    left_y += lh;
    item(left_x, left_y, "Q..I", "Select drum pattern 1-8", COLOR_PATTERN_SELECTED_FILL);
    left_y += lh;
  } else if (helpPage == 4) {
    heading(left_x, left_y, "Song Page");
    left_y += lh;
    heading(left_x, left_y, "Navigation");
    left_y += lh;
    item(left_x, left_y, "LEFT/RIGHT", "col / mode focus", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "UP/DOWN", "rows", COLOR_LABEL);
    left_y += lh;
    item(left_x, left_y, "ALT+UP/DN", "slot +/-", IGfxColor::Yellow());
    left_y += lh;

    heading(left_x, left_y, "Patterns");
    left_y += lh;
    item(left_x, left_y, "Q..I", "set 1-8", COLOR_PATTERN_SELECTED_FILL);
    left_y += lh;
    item(left_x, left_y, "BACK", "clear slot", IGfxColor::Red());
    left_y += lh;
  } else if (helpPage == 5) {
    heading(left_x, left_y, "Song Page (cont.)");
    left_y += lh;

    heading(left_x, left_y, "Playhead");
    left_y += lh;
    item(left_x, left_y, "ALT+UP/DN @PLAY", "nudge playhead", IGfxColor::Yellow());
    left_y += lh;
    left_y += lh;

    heading(left_x, left_y, "Mode");
    left_y += lh;
    item(left_x, left_y, "ENTER @ MODE", "Song/Pat toggle", IGfxColor::Green());
    left_y += lh;
    item(left_x, left_y, "M", "toggle mode", IGfxColor::Magenta());
    left_y += lh;
  }
}

void HelpPage::draw(IGfx& gfx, int x, int y, int w, int h) {
  drawHelpPage(gfx, x, y, w, h , help_page_index_);

  gfx.setTextColor(IGfxColor::Gray());
  gfx.drawLine(w, y, w, h);
  int total_height = h;
  int page_size = total_height / total_help_pages_;

  gfx.setTextColor(IGfxColor::White());
  int y1 = y + page_size * help_page_index_;
  int y2 = y1 + page_size;
  gfx.drawLine(w, y1 , w, y2);
}

bool HelpPage::handleEvent(UIEvent &ui_event) {
  bool handled = false;
  switch(ui_event.scancode){
    case MINIACID_UP:
      help_page_index_--;
      handled = true;
      break;
    case MINIACID_DOWN:
      help_page_index_++;
      handled = true;
      break;
    default:
      break;
  }
  if(help_page_index_ < 0) help_page_index_ = 0;
  if(help_page_index_ >= total_help_pages_) help_page_index_ = total_help_pages_ - 1;
  return handled;
}

const std::string & HelpPage::getTitle() const {
  static std::string title = "HELP";
  return title;
}
