#include "drum_sequencer_page.h"
#include <cctype>
#include <cstdio>
#include "../help_dialog.h"

DrumSequencerPage::DrumSequencerPage(IGfx& gfx, MiniAcid& mini_acid, AudioGuard& audio_guard)
 : gfx_(gfx),
   mini_acid_(mini_acid),
   audio_guard_(audio_guard),
   drum_step_cursor_(0),
   drum_voice_cursor_(0),
   drum_pattern_cursor_(0),
   drum_pattern_focus_(true) {
  int drumIdx = mini_acid_.currentDrumPatternIndex();
  if (drumIdx < 0 || drumIdx >= Bank<DrumPatternSet>::kPatterns) drumIdx = 0;
  drum_pattern_cursor_ = drumIdx;
}

int DrumSequencerPage::activeDrumPatternCursor() const {
  int idx = drum_pattern_cursor_;
  if (idx < 0) idx = 0;
  if (idx >= Bank<DrumPatternSet>::kPatterns)
    idx = Bank<DrumPatternSet>::kPatterns - 1;
  return idx;
}

int DrumSequencerPage::activeDrumStep() const {
  int idx = drum_step_cursor_;
  if (idx < 0) idx = 0;
  if (idx >= SEQ_STEPS) idx = SEQ_STEPS - 1;
  return idx;
}

int DrumSequencerPage::activeDrumVoice() const {
  int idx = drum_voice_cursor_;
  if (idx < 0) idx = 0;
  if (idx >= NUM_DRUM_VOICES) idx = NUM_DRUM_VOICES - 1;
  return idx;
}

void DrumSequencerPage::setDrumPatternCursor(int cursorIndex) {
  int cursor = cursorIndex;
  if (cursor < 0) cursor = 0;
  if (cursor >= Bank<DrumPatternSet>::kPatterns)
    cursor = Bank<DrumPatternSet>::kPatterns - 1;
  drum_pattern_cursor_ = cursor;
}

void DrumSequencerPage::moveDrumCursor(int delta) {
  if (mini_acid_.songModeEnabled()) {
    drum_pattern_focus_ = false;
  }
  if (drum_pattern_focus_) {
    int cursor = activeDrumPatternCursor();
    cursor = (cursor + delta) % Bank<DrumPatternSet>::kPatterns;
    if (cursor < 0) cursor += Bank<DrumPatternSet>::kPatterns;
    drum_pattern_cursor_ = cursor;
    return;
  }
  int step = activeDrumStep();
  step = (step + delta) % SEQ_STEPS;
  if (step < 0) step += SEQ_STEPS;
  drum_step_cursor_ = step;
}

void DrumSequencerPage::moveDrumCursorVertical(int delta) {
  if (delta == 0) return;
  if (mini_acid_.songModeEnabled()) {
    drum_pattern_focus_ = false;
  }
  if (drum_pattern_focus_) {
    if (delta > 0) {
      drum_pattern_focus_ = false;
    }
    return;
  }
  int voice = activeDrumVoice();
  int newVoice = voice + delta;
  if (newVoice < 0 || newVoice >= NUM_DRUM_VOICES) {
    drum_pattern_focus_ = true;
    drum_pattern_cursor_ = activeDrumStep() % Bank<DrumPatternSet>::kPatterns;
    return;
  }
  drum_voice_cursor_ = newVoice;
}

void DrumSequencerPage::focusPatternRow() { setDrumPatternCursor(drum_pattern_cursor_); drum_pattern_focus_ = true; }
void DrumSequencerPage::focusGrid() { drum_pattern_focus_ = false; drum_step_cursor_ = activeDrumStep(); drum_voice_cursor_ = activeDrumVoice(); }
bool DrumSequencerPage::patternRowFocused() const { if (mini_acid_.songModeEnabled()) return false; return drum_pattern_focus_; }

int DrumSequencerPage::patternIndexFromKey(char key) const {
  switch (std::tolower(static_cast<unsigned char>(key))) {
    case 'q': return 0; case 'w': return 1; case 'e': return 2; case 'r': return 3;
    case 't': return 4; case 'y': return 5; case 'u': return 6; case 'i': return 7;
    default: return -1;
  }
}

void DrumSequencerPage::withAudioGuard(const std::function<void()>& fn) {
  if (audio_guard_) { audio_guard_(fn); return; } fn();
}

// --- buffer & undo helpers ---
static inline void fetchHits(const MiniAcid& ma,
                             const bool*& kick, const bool*& snare, const bool*& hat, const bool*& openHat,
                             const bool*& midTom, const bool*& highTom, const bool*& rim, const bool*& clap) {
  kick    = ma.patternKickSteps();
  snare   = ma.patternSnareSteps();
  hat     = ma.patternHatSteps();
  openHat = ma.patternOpenHatSteps();
  midTom  = ma.patternMidTomSteps();
  highTom = ma.patternHighTomSteps();
  rim     = ma.patternRimSteps();
  clap    = ma.patternClapSteps();
}

void DrumSequencerPage::captureCurrentDrumPattern(DrumPatternState& out) const {
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  for (int v = 0; v < NUM_DRUM_VOICES; ++v)
    for (int s = 0; s < SEQ_STEPS; ++s)
      out.hits[v][s] = hits[v] ? hits[v][s] : false;
}

void DrumSequencerPage::applyDrumPatternState(const DrumPatternState& st) {
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      bool have = hits[v] ? hits[v][s] : false;
      bool want = st.hits[v][s];
      if (want != have) {
        withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s); });
      }
    }
  }
}

void DrumSequencerPage::pushUndo() {
  DrumPatternState st{}; captureCurrentDrumPattern(st);
  undo_stack_.push_back(st);
  if (undo_stack_.size() > static_cast<size_t>(kMaxHistory)) undo_stack_.erase(undo_stack_.begin());
  clearRedo();
}

void DrumSequencerPage::clearRedo() { redo_stack_.clear(); }

bool DrumSequencerPage::undo() {
  if (undo_stack_.empty()) return false;
  DrumPatternState cur{}; captureCurrentDrumPattern(cur);
  redo_stack_.push_back(cur);
  DrumPatternState prev = undo_stack_.back(); undo_stack_.pop_back();
  applyDrumPatternState(prev);
  return true;
}

bool DrumSequencerPage::redo() {
  if (redo_stack_.empty()) return false;
  DrumPatternState cur{}; captureCurrentDrumPattern(cur);
  undo_stack_.push_back(cur);
  DrumPatternState next = redo_stack_.back(); redo_stack_.pop_back();
  applyDrumPatternState(next);
  return true;
}

// --- pattern operations ---
void DrumSequencerPage::copyCurrentDrumPatternToBuffer() {
  DrumPatternState st{}; captureCurrentDrumPattern(st);
  for (int v = 0; v < NUM_DRUM_VOICES; ++v)
    for (int s = 0; s < SEQ_STEPS; ++s)
      buffer_.hits[v][s] = st.hits[v][s];
  buffer_.has_data = true;
}

void DrumSequencerPage::cutCurrentDrumPatternToBuffer() {
  pushUndo();
  copyCurrentDrumPatternToBuffer();
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      if (hits[v] && hits[v][s]) {
        withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s); });
      }
    }
  }
}

void DrumSequencerPage::pasteBufferToCurrentDrumPattern() {
  if (!buffer_.has_data) return;
  pushUndo();
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      bool have = hits[v] ? hits[v][s] : false;
      bool want = buffer_.hits[v][s];
      if (want != have) {
        withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s); });
      }
    }
  }
}

void DrumSequencerPage::transposeDrumInstruments(int dir) {
  // Rotate instrument rows: BD->SD->CH->OH->MT->HT->RS->CP
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  pushUndo();
  bool rotated[NUM_DRUM_VOICES][SEQ_STEPS];
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    int src = (v - dir) % NUM_DRUM_VOICES; if (src < 0) src += NUM_DRUM_VOICES;
    for (int s = 0; s < SEQ_STEPS; ++s) rotated[v][s] = hits[src] ? hits[src][s] : false;
  }
  // Apply differences
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      bool have = hits[v] ? hits[v][s] : false;
      bool want = rotated[v][s];
      if (want != have) withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s); });
    }
  }
}

void DrumSequencerPage::rotateDrumSteps(int dir) {
  // Horizontal rotation of steps per instrument
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  pushUndo();
  bool rotated[NUM_DRUM_VOICES][SEQ_STEPS];
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      int src = (s - dir) % SEQ_STEPS; if (src < 0) src += SEQ_STEPS;
      rotated[v][s] = hits[v] ? hits[v][src] : false;
    }
  }
  // Apply differences
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < SEQ_STEPS; ++s) {
      bool have = hits[v] ? hits[v][s] : false;
      bool want = rotated[v][s];
      if (want != have) withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s); });
    }
  }
}

void DrumSequencerPage::duplicateTopRowToBottomRow() {
  // Copy steps 0..7 -> 8..15 for all voices
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  pushUndo();
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    for (int s = 0; s < 8; ++s) {
      bool want = hits[v] ? hits[v][s] : false;
      bool have = hits[v] ? hits[v][s + 8] : false;
      if (want != have) withAudioGuard([&]() { mini_acid_.toggleDrumStep(v, s + 8); });
    }
  }
}

// --- event handling ---
bool DrumSequencerPage::handleEvent(UIEvent& ui_event) {
  if (ui_event.event_type != MINIACID_KEY_DOWN) return false;
  bool handled = false;
  switch (ui_event.scancode) {
    case MINIACID_LEFT:  moveDrumCursor(-1); handled = true; break;
    case MINIACID_RIGHT: moveDrumCursor( 1); handled = true; break;
    case MINIACID_UP:    moveDrumCursorVertical(-1); handled = true; break;
    case MINIACID_DOWN:  moveDrumCursorVertical( 1); handled = true; break;
    default: break;
  }
  if (handled) return true;

  char key = ui_event.key;
  if (!key) return false;

  // Enter: commit selection (pattern row) or toggle hit (grid)
  if ((key == '\n' || key == '\r')) {
    if (patternRowFocused()) {
      int cursor = activeDrumPatternCursor();
      withAudioGuard([&]() { mini_acid_.setDrumPatternIndex(cursor); });
    } else {
      int step = activeDrumStep();
      int voice = activeDrumVoice();
      withAudioGuard([&]() { mini_acid_.toggleDrumStep(voice, step); });
    }
    return true;
  }

  int patternIdx = patternIndexFromKey(key);
  if (patternIdx >= 0) {
    if (mini_acid_.songModeEnabled()) return true;
    focusPatternRow();
    setDrumPatternCursor(patternIdx);
    withAudioGuard([&]() { mini_acid_.setDrumPatternIndex(patternIdx); });
    return true;
  }

  // --- new keymap (matching synth page semantics) ---
  // D: transpose up (rotate instrument rows up)
  // C: transpose down (rotate instrument rows down)
  // F: rotate steps forward (right)
  // V: rotate steps backward (left)
  // H: copy pattern
  // N: paste pattern
  // J: undo
  // M: redo
  // '\\': cut pattern
  // '\'': duplicate 0..7 -> 8..15
  char lowerKey = static_cast<char>(std::tolower(static_cast<unsigned char>(key)));
  if (lowerKey == 'd') { transposeDrumInstruments(+1); return true; }
  if (lowerKey == 'c') { transposeDrumInstruments(-1); return true; }
  if (lowerKey == 'f') { rotateDrumSteps(+1); return true; }
  if (lowerKey == 'v') { rotateDrumSteps(-1); return true; }
  if (lowerKey == 'h') { copyCurrentDrumPatternToBuffer(); return true; }
  if (lowerKey == 'n') { pasteBufferToCurrentDrumPattern(); return true; }
  if (lowerKey == 'j') { if (undo()) return true; }
  if (lowerKey == 'm') { if (redo()) return true; }
  if (key == '\\')   { cutCurrentDrumPatternToBuffer(); return true; }
  if (key == '\'\'') { duplicateTopRowToBottomRow(); return true; }

  return false;
}

const std::string & DrumSequencerPage::getTitle() const {
  static std::string title = "DRUM SEQUENCER";
  return title;
}

void DrumSequencerPage::drawHelpBody(IGfx& gfx, int x, int y, int w, int h) {
  if (w <= 0 || h <= 0) return;
  switch (help_page_index_) {
    case 0: drawHelpPageDrumPatternEdit(gfx, x, y, w, h); break;
    default: break;
  }
  drawHelpScrollbar(gfx, x, y, w, h, help_page_index_, total_help_pages_);
}

bool DrumSequencerPage::handleHelpEvent(UIEvent& ui_event) {
  if (ui_event.event_type != MINIACID_KEY_DOWN) return false;
  int next = help_page_index_;
  switch (ui_event.scancode) {
    case MINIACID_UP:   next -= 1; break;
    case MINIACID_DOWN: next += 1; break;
    default: return false;
  }
  if (next < 0) next = 0;
  if (next >= total_help_pages_) next = total_help_pages_ - 1;
  help_page_index_ = next;
  return true;
}

bool DrumSequencerPage::hasHelpDialog() { return true; }

void DrumSequencerPage::draw(IGfx& gfx, int x, int y, int w, int h) {
  int body_y = y + 2;
  int body_h = h - 2;
  if (body_h <= 0) return;
  int pattern_label_h = gfx.fontHeight();
  gfx.setTextColor(COLOR_LABEL);
  gfx.drawText(x, body_y, "PATTERN");
  gfx.setTextColor(COLOR_WHITE);
  int spacing = 4;
  int pattern_size = (w - spacing * 7 - 2) / 8; if (pattern_size < 12) pattern_size = 12;
  int pattern_height = pattern_size / 2;
  int pattern_row_y = body_y + pattern_label_h + 1;
  int selectedPattern = mini_acid_.displayDrumPatternIndex();
  int patternCursor = activeDrumPatternCursor();
  bool songMode = mini_acid_.songModeEnabled();
  bool patternFocus = !songMode && patternRowFocused();
  if (songMode && selectedPattern >= 0) patternCursor = selectedPattern;
  for (int i = 0; i < Bank<DrumPatternSet>::kPatterns; ++i) {
    int col = i % 8;
    int cell_x = x + col * (pattern_size + spacing);
    bool isCursor = patternFocus && patternCursor == i;
    IGfxColor bg = songMode ? COLOR_GRAY_DARKER : COLOR_PANEL;
    gfx.fillRect(cell_x, pattern_row_y, pattern_size, pattern_height, bg);
    if (selectedPattern == i) {
      IGfxColor sel = songMode ? IGfxColor::Yellow() : COLOR_PATTERN_SELECTED_FILL;
      IGfxColor border = songMode ? IGfxColor::Yellow() : COLOR_LABEL;
      gfx.fillRect(cell_x - 1, pattern_row_y - 1, pattern_size + 2, pattern_height + 2, sel);
      gfx.drawRect(cell_x - 1, pattern_row_y - 1, pattern_size + 2, pattern_height + 2, border);
    }
    gfx.drawRect(cell_x, pattern_row_y, pattern_size, pattern_height, songMode ? COLOR_LABEL : COLOR_WHITE);
    if (isCursor) gfx.drawRect(cell_x - 2, pattern_row_y - 2, pattern_size + 4, pattern_height + 4, COLOR_STEP_SELECTED);
    char label[4]; std::snprintf(label, sizeof(label), "%d", i + 1);
    int tw = textWidth(gfx, label);
    int tx = cell_x + (pattern_size - tw) / 2;
    int ty = pattern_row_y + pattern_height / 2 - gfx.fontHeight() / 2;
    gfx.setTextColor(songMode ? COLOR_LABEL : COLOR_WHITE);
    gfx.drawText(tx, ty, label);
    gfx.setTextColor(COLOR_WHITE);
  }
  int grid_top = pattern_row_y + pattern_height + 6;
  int grid_h = body_h - (grid_top - body_y); if (grid_h <= 0) return;
  int label_w = 18;
  int grid_x = x + label_w;
  int grid_w = w - label_w; if (grid_w < 8) grid_w = 8;
  const char* voiceLabels[NUM_DRUM_VOICES] = {"BD", "SD", "CH", "OH", "MT", "HT", "RS", "CP"};
  int labelStripeH = grid_h / NUM_DRUM_VOICES; if (labelStripeH < 3) labelStripeH = 3;
  for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
    int ly = grid_top + v * labelStripeH + (labelStripeH - gfx.fontHeight()) / 2;
    gfx.setTextColor(COLOR_LABEL);
    gfx.drawText(x, ly, voiceLabels[v]);
  }
  gfx.setTextColor(COLOR_WHITE);
  int cursorStep = activeDrumStep();
  int cursorVoice = activeDrumVoice();
  bool gridFocus = !patternFocus;
  const int cell_w = grid_w / SEQ_STEPS; if (cell_w < 2) return;
  const bool *kick, *snare, *hat, *openHat, *midTom, *highTom, *rim, *clap;
  fetchHits(mini_acid_, kick, snare, hat, openHat, midTom, highTom, rim, clap);
  int highlight = mini_acid_.currentStep();
  const bool* hits[NUM_DRUM_VOICES] = {kick, snare, hat, openHat, midTom, highTom, rim, clap};
  const IGfxColor colors[NUM_DRUM_VOICES] = {COLOR_DRUM_KICK, COLOR_DRUM_SNARE, COLOR_DRUM_HAT, COLOR_DRUM_OPEN_HAT,
                                             COLOR_DRUM_MID_TOM, COLOR_DRUM_HIGH_TOM, COLOR_DRUM_RIM, COLOR_DRUM_CLAP};
  int stripe_h = grid_h / NUM_DRUM_VOICES; if (stripe_h < 3) stripe_h = 3;
  for (int i = 0; i < SEQ_STEPS; ++i) {
    int cw = cell_w;
    int ch = stripe_h; if (ch < 3) ch = 3;
    int cx = grid_x + i * cw;
    for (int v = 0; v < NUM_DRUM_VOICES; ++v) {
      int cy = grid_top + v * stripe_h;
      bool hit = hits[v] ? hits[v][i] : false;
      IGfxColor fill = hit ? colors[v] : COLOR_GRAY;
      gfx.fillRect(cx, cy, cw - 1, ch - 1, fill);
      if (highlight == i) gfx.drawRect(cx - 1, cy - 1, cw + 1, ch + 1, COLOR_STEP_HILIGHT);
      if (gridFocus && i == cursorStep && v == cursorVoice) gfx.drawRect(cx, cy, cw - 1, ch - 1, COLOR_STEP_SELECTED);
    }
  }
}
