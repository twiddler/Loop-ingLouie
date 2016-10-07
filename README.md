# Loop()ingLouie
This project is in German. Please drop us a line if you'd like to see it translated.

**Loop()ingLouie macht [Looping Louie](https://www.google.de/search?q=looping%20louie) wieder krass!** Schneller fahren, rückwärts fahren, oder einfach mal anhalten ... whaaat? Aber macht euch selbst ein Bild. Mit versteckter Kamera haben wir einen frühen Prototypen filmen können: [Link zum Video folgt]

## Hardware
Unser Controller besteht aus einem Arduino Nano, 3 LEDs, einem Powerswitch, und einem Knopf. Details folgen noch.

## Logik
Das Herzstück des Codes ist ein Array von Funktionen, die die Spielmodi darstellen. Sie liefern einen float zwischen -1 und 1 (≙ "Vollgas rückwärts" und "Vollgas vorwärts"). Zwei Beispielmodi:
```c++
typedef float (*Spielmodus) ();
Spielmodus spielmodi[] = {
  []() -> float {
    // Vollgas
    return 1.0f;
  },
  
  []() -> float {
    // Zwischen zwei Geschwindigkeiten sinus-förmig oszillieren
    const float pi = static_cast<float>(atan(1)*4);
    const float low = 0.24f;
    const float high = 1.0f;
    return clip(static_cast<float>((high-low)/2+low + sin(laufzeit_spielmodus_get()/(2*pi*130))*((high-low)/2)), low, high);
  },
  
  []() -> float {
    // ...
  }
};
```

Es sind natürlich auch kompliziertere Funktionen möglich:
```c++
bremse bis zum Stehenbleiben
warte eine zufällige Zeit lang
fahre vorwärts mit Vollgas
```

Im loop() wird mit der aktuell ausgewählten Funktion die Geschwindigkeit bestimmt und am Motor eingestellt:
```c++

void loop() {
  static Button button(12); // Button an Pin 12
  static const Motor motor(6, 5); // Motor an Pins 6 und 5

  // Bei Knopfdruck nächsten Spielmodus auswählen
  button.status_abfragen();
  if (button.wurde_losgelassen()) {
    spielmodus = (spielmodus + 1) % anzahl_spielmodi; // Den nächsten Spielmodus auswählen
  }

  // Geschwindigkeit je nach Spielmodus einstellen
  motor.geschwindigkeit_set(spielmodi[spielmodus]());
}
```

## Fragen?
Dann schreibt uns einfach! (:
