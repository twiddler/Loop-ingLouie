/*
  Kontrolliert einen Button
*/
#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {
  public:
    Button(unsigned int pin);
    boolean wurde_gedrueckt() const;
    boolean wurde_losgelassen() const;
    void status_abfragen();
  private:
    unsigned int _pin;
    boolean _zuletzt_ungedrueckt;
    boolean _wurde_gedrueckt;
    boolean _wurde_losgelassen;
};

#endif