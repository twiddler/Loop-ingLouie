#include <limits.h> // http://www.cplusplus.com/reference/climits/
#include <Led.h> // Klasse Pin erzeugt für einen Pin eine Led, die dann an- und ausgeschaltet werden kann
#include <Motor.h> // Klasse Motor erzeugt für zwei Pins einen Motor, dessen Geschwindigkeit und maximale Geschwindigkeit kontrolliert werden kann
#include <Button.h> // Klasse Button erzeugt für einen Pin einen Button, der gedrückt und losgelassen werden kann
#include <Stoppuhr.h> // Überprüft, ob eine gewisse Zeit verstrichen ist
#include <Intervall.h> // Ein Intervall, aus dem ein zufälliger Wert genommen werden kann


/* 
 *  Allgemeine Funktionen
 */
 
// Gibt die Arraylänge zurück
// https://arduino.land/FAQ/content/6/29/en/how-to-get-the-size-of-an-array.html
#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) ) // Gibt die Länge eines Arrays zurück

// Zwingt den Wert n ins geschlossene Intervall [lower, upper]
// http://stackoverflow.com/a/9324086
template <typename T> T clip(const T& n, const T& lower, const T& upper) {
  return max(lower, min(n, upper));
}

// This allows you to write succinct code like
// Serial << "My name is " << name << " and I am " << age << " years old.";
// http://playground.arduino.cc/Main/StreamingOutput
template<class T> inline Print &operator <<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}


/* 
 *  Globale Variablen
 */

const Led leds[] = {Led(9), Led(10), Led(11)}; // LEDs für die Pins erstellen
unsigned int spielmodus = 3;
unsigned long laufzeit_spielmodus_beginn = 0;
unsigned int anzahl_spielmodi = 0;
const boolean debugging = true;


/* 
 *  LED-bezogene Funktionen
 */

// Setzt alle LEDs auf an (true) oder aus (false)
inline void alle_leds(boolean led_status) {
  for (Led led: leds) {
    led.status_set(led_status);
  }
}

// Lässt alle LEDs für eine bestimmte Anzahl (count), Dauer (time_on) und Pausendauer (time_off) blinken
inline void alle_leds_blinken(int count = 3, int time_on = 120, int time_off = 120) {
  for (int i = 0; i < count; i++) {
    alle_leds(1);
    delay(time_on);
    alle_leds(0);
    delay(time_off);
  }
}


/* 
 *  Spielmodi
 */

// Funktionen für die Laufzeit des aktuellen Spielmodus
#define laufzeit_spielmodus_set()  ( laufzeit_spielmodus_beginn = millis() ) // Setzt die Startzeit des aktuellen Spielmodus auf die aktuelle Laufzeit des Programms
#define laufzeit_spielmodus_get()  ( millis() - laufzeit_spielmodus_beginn ) // Gibt die Laufzeit des aktuellen Spielmodus

// Hier werden die Spielmodi programmiert. Für jeden Spielmodus gibt es im Array eine Funktion, die die aktuelle Geschwindigkeit bestimmt.
typedef float (*Spielmodus) ();
Spielmodus spielmodi[] = {
  []() -> float {
    // Vollgas
    return 1.0f;
  },
  
  []() -> float {
    // Zwischen zwei Geschwindigkeiten sinus-förmig oszillieren
    // Dieser Spielmodus wird auch in Spielmodus 3 benutzt, ggf. also vorm Verändern dorthin kopieren
    const float pi = static_cast<float>(atan(1)*4); // http://stackoverflow.com/a/1727886
    const float low = 0.24f;
    const float high = 1.0f;
    return clip(static_cast<float>((high-low)/2+low + sin(laufzeit_spielmodus_get()/(2*pi*130))*((high-low)/2)), low, high);
  },
  
  []() -> float {
    // Nach einer festen Zeit einen Spieler angreifen (auf einer Stelle hin und her)
    {
      const unsigned int vorwaerts = 7000; // So viele ms wird zunächst normal gefahren (s.u.)
      const unsigned int rueckwaerts = 3000; // Dann wird auf der Stelle hin- und hergefahren
      const unsigned int hinundher = 3; // So oft wird hin- und hergefahren
      const float geschwindigkeit = 0.6f; // Geschwindigkeit beim Vor- und Rückwärtsfahren (bei Vollgas rutscht der Arm durch)
  
      // Stoppuhren erstellen
      static Stoppuhr hinundher_vorwaerts(vorwaerts);
      static Stoppuhr hinundher_rueckwaerts(vorwaerts + rueckwaerts);
      
      if (!hinundher_vorwaerts.ist_abgelaufen()) {
        // Eine Zeit lang normal fahren (hier kommt kein Code hin)
      } else if (!hinundher_rueckwaerts.ist_abgelaufen()) {
        // Dann auf einer Stelle hin- und herfahren
        const float oscillator = (((vorwaerts + rueckwaerts - hinundher_rueckwaerts.laeuft_bereits()) % ((rueckwaerts)/hinundher)) / ((static_cast<float>(rueckwaerts))/hinundher) - 0.5f) * 2; // oscillator läuft linear zwischen 1.0f und -1.0f hin- und her
        return (oscillator < 0 ? floor(oscillator) : ceil(oscillator)) * geschwindigkeit; // Rück- oder vorwärts mit oben eingestellter Geschwindigkeit
      } else {
        // Ein Zyklus ist rum, Stoppuhren neu starten
        hinundher_vorwaerts.reset();
        hinundher_rueckwaerts.reset();
      }
    }

    // Wie Spielmodus 2 fahren, nur ab und zu ein Stück rückwärts
    {
      // In welchem Zeitraum für wie lange wie schnell zurückgefahren werden soll
      const Intervall<unsigned int> vorwaerts_intervall(2000, 7000); // Zwischen a und b ms dürfen vergehen
      const Intervall<unsigned int> rueckwaerts_intervall(270, 400); // Für c bis d ms wird zurückgefahren
      const float low = -1.0f; // maximale Rückwärtsgeschwindigkeit
      const float high = -0.3f; // minimale Rückwärtsgeschwindigkeit

      // Stoppuhren stellen
      static const unsigned int vorwaerts = vorwaerts_intervall.zufallswert();
      static Stoppuhr ueberraschung_vorwaerts(vorwaerts);
      static Stoppuhr ueberraschung_rueckwaerts(vorwaerts + rueckwaerts_intervall.zufallswert());

      // Je nach Stoppuhr entscheiden
      if (!ueberraschung_vorwaerts.ist_abgelaufen()) {
        // Wird nur gebraucht, damit unten zur richtigen Zeit ins else gesprungen wird
      } else if (!ueberraschung_rueckwaerts.ist_abgelaufen()) {
        // Rückwärts fahren, zufällig schnell
        return low + static_cast <float> (rand()) /(static_cast <float> (RAND_MAX/(high-low)));
      } else {
        // Ein Zyklus ist rum, Timer neu starten und nächsten Anschlag planen
        const unsigned int vorwaerts = vorwaerts_intervall.zufallswert();
        ueberraschung_vorwaerts = Stoppuhr(vorwaerts);
        ueberraschung_rueckwaerts = Stoppuhr(vorwaerts + rueckwaerts_intervall.zufallswert());
      }
      return spielmodi[1](); // Spielmodus 2
    }
  },
  
  []() -> float {
    // Vorwärtsfahren und bremsen bis zum Anhalten, zufällig warten, dann Vollgas
    const unsigned int normal = 7000; // So viele ms wird zunächst normal gefahren (s.u.)
    const Intervall<unsigned int> warten(200, 3000); // Zufällige Zeit wird gewartet
    const unsigned int vollgas = 400; // Solange Vollgas
    const float geschwindigkeit = 0.7f; // normale Geschwindigkeit

    // Stoppuhren erstellen
    static Stoppuhr uhr_normal(normal);
    static unsigned int warten_r = warten.zufallswert();
    static Stoppuhr uhr_warten(normal + warten_r);
    static Stoppuhr uhr_vollgas(normal + warten_r + vollgas);
    
    if (!uhr_normal.ist_abgelaufen()) {
      // Eine Zeit lang normal fahren (hier kommt kein Code hin)
    } else if (!uhr_warten.ist_abgelaufen()) {
      return 0.0f;
    } else if (!uhr_vollgas.ist_abgelaufen()) {
      return 1.0f;
    } else {
      // Ein Zyklus ist rum, Stoppuhren neu starten
      uhr_normal.reset();
      warten_r = warten.zufallswert();
      uhr_warten = Stoppuhr(normal + warten_r);
      uhr_vollgas = Stoppuhr(normal + warten_r + vollgas);
    }
    return geschwindigkeit;
  },
  
  []() -> float {
    // Aus den anderen Spielmodi zufällig einen aussuchen
    static int i;
    static boolean first = true;

    static Stoppuhr wechsel(11000);
    if (!first && !wechsel.ist_abgelaufen()) {
      // Wird nur gebraucht, damit unten zur richtigen Zeit ins else gesprungen wird
    } else {
      // Ein Zyklus ist rum, Timer neu starten und den nächsten Spielmodus auswählen
      first = false;
      const int i_alt = i;
      do {i = rand() % (int)(anzahl_spielmodi);} while ((i == 4) || (i == i_alt)); // Einen neuen Modus auswählen, der nicht der hier ist
      wechsel.reset();
    }
    return spielmodi[i](); // Spielmodus 2
  }
};


/* 
 *  setup, once, loop
 */

// Arduino starten
void setup() {
  // Serielle Verbindung fürs Debuggen aufbauen
  Serial.begin(9600);
  Serial.println("Ich bin jetzt an. (setup laeuft)");

  anzahl_spielmodi = arr_len(spielmodi);
  
  once();
}

// Aktionen, die einmalig nach dem Start ausgeführt werden
void once() {
  // LEDs beim Start aufleuchten lassen
  if (!debugging) { // Überspringen, wenn du nur am Rest rumschreibst
    for (Led led: leds) {
      led.on();
      delay(400);
    }
    delay(200);
    for (int j = 0; j < 3; j++) {
      alle_leds(1);
      delay(120);
      alle_leds(0);
      delay(120);
    }
    alle_leds(1);
    delay(600);
    leds[1].off();
    delay(300);
    leds[2].off();
    delay(100);
    leds[0].off();
  }
}

// Aktionen, die nach dem Start immer wieder ausgeführt werden
void loop() {
  static unsigned int led = 0;
  static boolean an = true;
  static Stoppuhr stoppuhr_ledlauf(200);
  static Button button(12);
  static const Motor motor(6, 5); // Motor für die Pins erstellen

  // Aktivität anzeigen: LEDs nacheinander umschalten
  if (stoppuhr_ledlauf.ist_abgelaufen()) {
    leds[led].status_set(an); // Die aktuelle LED umschalten
    led = (led + 1) % arr_len(leds); // Die nächste LED auswählen

    // Wenn led == 0 ist, soll an gewechselt werden
    if (led == 0) {
      an = !an;
    }
    // Eine etwas langsamere, aber deutlich coolere Lösung ist die folgende:
    //   an = (led == 0) == !an;
    // Das funktioniert tatsächlich. :o Die Fälle:
    //   led == 0, an == 0:  an = 1 == 1  ==  1
    //   led == 1, an == 0:  an = 0 == 1  ==  0
    //   led == 0, an == 1:  an = 1 == 0  ==  0
    //   led == 1, an == 1:  an = 0 == 0  ==  1
    
    stoppuhr_ledlauf.reset(); // Neu stoppen
  }

  // Wurde der Knopf gedrückt?
  button.status_abfragen();
  if (button.wurde_losgelassen()) {
    // Nächsten Spielmodus starten
    motor.geschwindigkeit_set(0.0f); // Aktuellen Modus anhalten
    alle_leds(0); // LEDs ausschalten
    delay(300);
    spielmodus = (spielmodus + 1) % anzahl_spielmodi; // // Den nächsten Spielmodus auswählen
    alle_leds_blinken(spielmodus + 1); // LEDs so oft blinken lassen, wie hoch der Spielmodus ist
    delay(180);
    laufzeit_spielmodus_set(); // Nachhalten, wie lange der Spielmodus schon läuft
    
    // Die Blinksequenz neustarten
    an = true;
    led = 0;
    stoppuhr_ledlauf.reset();
  }

  // Geschwindigkeit je nach Spielmodus einstellen
  motor.geschwindigkeit_set(spielmodi[spielmodus]());
}
