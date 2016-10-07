/*
  Kontrolliert eine LED
*/

#include "Arduino.h"
#include "Led.h"

Led::Led(unsigned int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void Led::on() const {
  status_set(1);
}

void Led::off() const {
  status_set(0);
}

void Led::status_set(boolean status) const {
  digitalWrite(_pin, status);
}