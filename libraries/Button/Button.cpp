/*
  Kontrolliert einen Button
*/

#include "Arduino.h"
#include "Button.h"

Button::Button(unsigned int pin) {
  _pin = pin;
  _zuletzt_ungedrueckt = true;
}

void Button::status_abfragen() {
  boolean ungedrueckt = digitalRead(_pin);
  _wurde_gedrueckt = _zuletzt_ungedrueckt && !ungedrueckt;
  _wurde_losgelassen = !_zuletzt_ungedrueckt && ungedrueckt;
  _zuletzt_ungedrueckt = ungedrueckt; // letzten Zustand des Buttons speichern
}

boolean Button::wurde_gedrueckt() const {
  return _wurde_gedrueckt;
}

boolean Button::wurde_losgelassen() const {
  return _wurde_losgelassen;
}