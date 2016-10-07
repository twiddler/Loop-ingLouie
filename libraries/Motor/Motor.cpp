/*
  Kontrolliert einen Motor
*/

#include "Arduino.h"
#include "Motor.h"

// Gibt das Vorzeichen einer Variable (int, float, double, ...) zurück
// http://stackoverflow.com/a/4609795
template <typename T> int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

// Zwingt den Wert n ins geschlossene Intervall [lower, upper]
// http://stackoverflow.com/a/9324086
template <typename T> T clip(const T& n, const T& lower, const T& upper) {
  return max(lower, min(n, upper));
}

Motor::Motor(unsigned int pin_vorwaerts, unsigned int pin_rueckwaerts, int geschwindigkeit_max) {
  pinMode(pin_vorwaerts, OUTPUT);
  pinMode(pin_rueckwaerts, OUTPUT);
  _pin_vorwaerts = pin_vorwaerts;
  _pin_rueckwaerts = pin_rueckwaerts;
  _geschwindigkeit_max = geschwindigkeit_max;
}

void Motor::geschwindigkeit_set(float geschwindigkeit) const {
  boolean dreht_vorwaerts = sgn(geschwindigkeit) > 0;
  int geschwindigkeit_umgerechnet_abs = clip(static_cast<int>(fabsf(geschwindigkeit) * _geschwindigkeit_max), 0, _geschwindigkeit_max); // http://stackoverflow.com/questions/2544394/c-floating-point-to-integer-type-conversions
  analogWrite(_pin_rueckwaerts, !dreht_vorwaerts * geschwindigkeit_umgerechnet_abs); // für Rückwärtsdrehungen
  analogWrite(_pin_vorwaerts, dreht_vorwaerts * geschwindigkeit_umgerechnet_abs); // für Vorwärtsdrehungen
  Serial.println(dreht_vorwaerts ? "    -->" : geschwindigkeit_umgerechnet_abs == 0.0f ? "   o" : "<--");
}