/*
  Überprüft, ob eine gewisse Zeit verstrichen ist
*/
#ifndef Stoppuhr_h
#define Stoppuhr_h

#include "Arduino.h"

class Stoppuhr {
  public:
    Stoppuhr(unsigned long minimale_pause) {
      _minimale_pause = minimale_pause;
    };
    boolean ist_abgelaufen() const {
      return (millis() - _letzter_stand) >= _minimale_pause;
    };
    unsigned long laeuft_bereits() const {
      return millis() - _letzter_stand;
    }
    void reset() {
      _letzter_stand = millis();
    }
  private:
    unsigned long _letzter_stand = millis();
    unsigned long _minimale_pause;
};

#endif
