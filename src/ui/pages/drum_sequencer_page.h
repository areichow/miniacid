#pragma once
#include <functional>
#include <vector>
#include "../ui_core.h"
#include "../ui_colors.h"
#include "../ui_utils.h"

class DrumSequencerPage : public IPage {
 public:
  DrumSequencerPage(IGfx& gfx, MiniAcid& mini_acid, AudioGuard& audio_guard);
  void draw(IGfx& gfx, int x, int y, int w, int h) override;
  void drawHelpBody(IGfx& gfx, int x, int y, int w, int h) override;
  bool handleEvent(UIEvent& ui_event) override;
  bool handleHelpEvent(UIEvent& ui_event) override;
  const std::string & getTitle() const override;
  bool hasHelpDialog() override;

 private:
  int activeDrumPatternCursor() const;
  int activeDrumStep() const;
  int activeDrumVoice() const;
  void setDrumPatternCursor(int cursorIndex);
  void moveDrumCursor(int delta);
  void moveDrumCursorVertical(int delta);
  void focusPatternRow();
  void focusGrid();
  bool patternRowFocused() const;
  int patternIndexFromKey(char key) const;
  void withAudioGuard(const std::function<void()>& fn);

  // pattern ops
  void copyCurrentDrumPatternToBuffer();
  void cutCurrentDrumPatternToBuffer();
  void pasteBufferToCurrentDrumPattern();
  void transposeDrumInstruments(int dir);   // dir = +1 up, -1 down
  void rotateDrumSteps(int dir);            // dir = +1 forward (right), -1 backward (left)
  void duplicateTopRowToBottomRow();        // steps 0..7 -> 8..15 for all voices

  // undo/redo
  struct DrumPatternState {
    bool hits[NUM_DRUM_VOICES][SEQ_STEPS];
  };
  static constexpr int kMaxHistory = 64;
  void captureCurrentDrumPattern(DrumPatternState& out) const;
  void applyDrumPatternState(const DrumPatternState& st);
  void pushUndo();
  void clearRedo();
  bool undo();
  bool redo();

  IGfx& gfx_;
  MiniAcid& mini_acid_;
  AudioGuard& audio_guard_;
  int drum_step_cursor_;
  int drum_voice_cursor_;
  int drum_pattern_cursor_;
  bool drum_pattern_focus_;
  int help_page_index_ = 0;
  int total_help_pages_ = 1;

  // buffer
  struct DrumBuffer {
    bool has_data = false;
    bool hits[NUM_DRUM_VOICES][SEQ_STEPS] = { {false} };
  } buffer_;

  // undo/redo stacks
  std::vector<DrumPatternState> undo_stack_;
  std::vector<DrumPatternState> redo_stack_;
};
