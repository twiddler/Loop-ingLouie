/*
  Kontrolliert einen Motor
*/
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor {
  public:
    Motor(unsigned int pin_vorwaerts, unsigned int pin_rueckwaerts, int geschwindigkeit_max = 255);
    void geschwindigkeit_set(float geschwindigkeit) const;
  private:
    unsigned int _pin_rueckwaerts;
    unsigned int _pin_vorwaerts;
    int _geschwindigkeit_max;
};

#endif
