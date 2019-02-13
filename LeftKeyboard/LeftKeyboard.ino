/* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/

#include <Keypad.h>

#define KEY_NON_US_BS  (100 | 0xF000)
#define KEY_MENU       (101 | 0xF000)

const byte ROWS = 7; //four rows
const byte COLS = 6; //three columns
const int LAYOUT_NBR = 1;

uint16_t layouts[LAYOUT_NBR][ROWS*COLS]{
  {
    KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, MODIFIERKEY_SHIFT, MODIFIERKEY_CTRL,
    NO_KEY,  KEY_1,     KEY_Q,   KEY_A,         KEY_NON_US_BS,     MODIFIERKEY_GUI,
    KEY_F1,  KEY_2,     KEY_W,   KEY_S,         KEY_Z,             NO_KEY,
    KEY_F2,  KEY_3,     KEY_E,   KEY_D,         KEY_X,             MODIFIERKEY_ALT,
    KEY_F3,  KEY_4,     NO_KEY,  NO_KEY,        KEY_C,             KEY_SPACE,
    KEY_F4,  KEY_5,     KEY_R,   KEY_F,         KEY_V,             NO_KEY,
    NO_KEY,  KEY_6,     KEY_T,   KEY_G,         KEY_B,             NO_KEY
  }
};

char keys[ROWS][COLS] = {
{'A','B','C', 'D', 'E', 'F'},
{'G','H','I', 'J', 'K', 'L'},
{'M','N','O', 'P', 'Q', 'R'},
{'S','T','U', 'V', 'W', 'X'},
{'Y','Z','a', 'b', 'c', 'd'},
{'e','f','g', 'h', 'i', 'j'},
{'k','l','m', 'n', 'o', 'p'}
};
byte colPins[COLS] = {21, 20, 19, 18, 17, 16}; //connect to the row pinouts of the kpd
byte rowPins[ROWS] = { 9, 10, 11, 13, 14, 15, 12}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;
byte currentLayout = 0;

void setup() {
    Serial.begin(9600);
    loopCount = 0;
    startTime = millis();
    msg = "";
}


void loop() {
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        Serial.print("Average loops per second = ");
        Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            char keyPressed = kpd.key[i].kchar;
            uint16_t keyChar = layouts[currentLayout][kpd.key[i].kcode];
            
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {             
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    Keyboard.press(keyChar);
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    Keyboard.release(keyChar);
                    msg = " RELEASED.";
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print("Key ");
                Serial.print(keyPressed);
                Serial.println(msg);
            }
        }
    }
}  // End loop
