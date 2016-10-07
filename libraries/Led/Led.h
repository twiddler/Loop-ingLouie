/*
  Kontrolliert eine LED
*/
#ifndef Led_h
#define Led_h

#include "Arduino.h"

class Led {
  public:
    Led(unsigned int pin);
    void on() const;
    void off() const;
    void status_set(boolean status) const;
  private:
    unsigned int _pin;
};

#endif
