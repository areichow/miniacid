#pragma once

class Parameter {
public:
  Parameter();
  Parameter(const char* label, const char* unit, float minValue, float maxValue, float defaultValue, float step);

  const char* label() const;
  const char* unit() const;
  float value() const;
  float min() const;
  float max() const;
  float step() const;
  float normalized() const;

  void setValue(float v);
  void addSteps(int steps);
  void setNormalized(float norm);
  void reset();

private:
  const char* _label;
  const char* _unit;
  float min_;
  float max_;
  float default_;
  float step_;
  float value_;
};

inline Parameter::Parameter()
  : _label(""), _unit(""), min_(0.0f), max_(1.0f),
    default_(0.0f), step_(0.0f), value_(0.0f) {}

inline Parameter::Parameter(const char* label, const char* unit, float minValue, float maxValue, float defaultValue, float step)
  : _label(label), _unit(unit), min_(minValue), max_(maxValue),
    default_(defaultValue), step_(step), value_(defaultValue) {}

inline const char* Parameter::label() const { return _label; }
inline const char* Parameter::unit() const { return _unit; }
inline float Parameter::value() const { return value_; }
inline float Parameter::min() const { return min_; }
inline float Parameter::max() const { return max_; }
inline float Parameter::step() const { return step_; }

inline float Parameter::normalized() const {
  if (max_ <= min_) return 0.0f;
  return (value_ - min_) / (max_ - min_);
}

inline void Parameter::setValue(float v) {
  if (v < min_) v = min_;
  if (v > max_) v = max_;
  value_ = v;
}

inline void Parameter::addSteps(int steps) {
  setValue(value_ + step_ * steps);
}

inline void Parameter::setNormalized(float norm) {
  if (norm < 0.0f) norm = 0.0f;
  if (norm > 1.0f) norm = 1.0f;
  value_ = min_ + norm * (max_ - min_);
}

inline void Parameter::reset() {
  value_ = default_;
}

