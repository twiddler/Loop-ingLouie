/*
  Ein Intervall, aus dem ein zufälliger Wert genommen werden kann
*/
#ifndef Interval_h
#define Interval_h

#include "Arduino.h"

template <class T>
class Intervall {
  public:
    Intervall(T low, T high) {
      _low = low;
      _high = high;
    };
    T zufallswert() const {
      Serial.println(_low + (static_cast<T>(rand()) % (_high - _low + 1)));
      return _low + (static_cast<T>(rand()) % (_high - _low + 1));
    };
  private:
    T _low = 0;
    T _high;
};

#endif
