#include <Keypad.h>
#include <Wire.h>

const byte ROWS = 11;
const byte COLS = 6;
const byte I2C_ADDRESS = 8;

char keys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'G', 'H', 'I', 'J', 'K', 'L'},
  {'M', 'N', 'O', 'P', 'Q', 'R'},
  {'S', 'T', 'U', 'V', 'W', 'X'},
  {'Y', 'Z', 'a', 'b', 'c', 'd'},
  {'e', 'f', 'g', 'h', 'i', 'j'},
  {'k', 'l', 'm', 'n', 'o', 'p'},
  {'q', 'r', 's', 't', 'u', 'v'},
  {'w', 'x', 'y', 'z', '+', '"'},
  {'*', '[', '%', '&', '/', '('},
  {'A', 'B', 'C', 'D', 'E', 'F'},
};
byte colPins[COLS] = {21, 20, 19, 18, 17, 16};
byte rowPins[ROWS] = { 0, 1, 4, 3, 9, 10, 22, 12, 13, 14, 15};

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long loopCount;
unsigned long startTime;
String msg;
byte keyOr;

void setup() {
  Serial.begin(9600);
    
  Wire.begin();
    
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

  if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            char keyPressed = kpd.key[i].kchar;
            int keyCode = kpd.key[i].kcode;
            
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {             
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    Wire.beginTransmission(I2C_ADDRESS);
                    Wire.write(keyCode);
                    Wire.endTransmission();
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    Wire.beginTransmission(I2C_ADDRESS);
                    keyOr = keyCode | 0x80;
                    Wire.write(keyOr);
                    Wire.endTransmission();
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
}
