#include <Keypad.h>
#include <Wire.h>

//#define KEY_NON_US_BS  (100 | 0xF000)
//#define KEY_MENU       (101 | 0xF000)
//#define KEY_FN 1

#define STR_PRODUCT "Gimly Split Mech Keyboard"

const byte LEFT_COLS = 7;
const byte LEFT_ROWS = 6;
const byte RIGHT_COLS = 11;
const byte RIGHT_ROWS = 6;
const int LAYOUT_NBR = 2;
const byte I2C_ADDRESS = 8;

uint16_t layoutsRight[LAYOUT_NBR][RIGHT_COLS*RIGHT_ROWS]{
  {
    KEY_ESC, KEY_TILDE, KEY_TAB, KEY_CAPS_LOCK, MODIFIERKEY_SHIFT, MODIFIERKEY_CTRL,
    NO_KEY,  KEY_1,     KEY_Q,   KEY_A,         KEY_NON_US_BS,     MODIFIERKEY_GUI,
    KEY_F1,  KEY_2,     KEY_W,   KEY_S,         KEY_Z,             NO_KEY,
    KEY_F2,  KEY_3,     KEY_E,   KEY_D,         KEY_X,             MODIFIERKEY_ALT,
    KEY_F3,  KEY_4,     NO_KEY,  NO_KEY,        KEY_C,             KEY_SPACE,
    KEY_F4,  KEY_5,     KEY_R,   KEY_F,         KEY_V,             NO_KEY,
    NO_KEY,  KEY_6,     KEY_T,   KEY_G,         KEY_B,             NO_KEY
  },
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

uint16_t layoutsLeft[LAYOUT_NBR][RIGHT_COLS*RIGHT_ROWS]{
  {
    KEY_F5,          KEY_7,         KEY_Y,           KEY_H,         KEY_N,                   KEY_SPACE,
    KEY_F6,          KEY_8,         KEY_U,           KEY_J,         KEY_M,                   MODIFIERKEY_RIGHT_ALT,
    KEY_F7,          KEY_9,         KEY_I,           KEY_K,         KEY_COMMA,               NO_KEY,
    KEY_F8,          KEY_0,         KEY_O,           KEY_L,         KEY_PERIOD,              1,
    KEY_F9,          KEY_MINUS,     KEY_P,           KEY_SEMICOLON, KEY_SLASH,               MODIFIERKEY_RIGHT_GUI,
    KEY_F10,         KEY_EQUAL,     KEY_LEFT_BRACE,  KEY_QUOTE,     NO_KEY,                  NO_KEY,
    KEY_F11,         KEY_BACKSPACE, KEY_RIGHT_BRACE, KEY_BACKSLASH, MODIFIERKEY_RIGHT_SHIFT, KEY_MENU,
    KEY_F12,         NO_KEY,        KEY_ENTER,       NO_KEY,        NO_KEY,                  MODIFIERKEY_RIGHT_CTRL,
    KEY_PRINTSCREEN, KEY_INSERT,    KEY_DELETE,      NO_KEY,        NO_KEY,                  KEY_LEFT,
    KEY_SCROLL_LOCK, KEY_HOME,      KEY_END,         NO_KEY,        KEY_UP,                  KEY_DOWN,
    KEY_PAUSE,       KEY_PAGE_UP,   KEY_PAGE_DOWN,   NO_KEY,        NO_KEY,                  KEY_RIGHT
  },
  {
    KEY_F5,          KEY_7,         KEY_Y,           KEY_H,         KEY_N,                   0,
    KEY_F6,          KEYPAD_SLASH,  KEY_U,           KEY_J,         KEY_M,                   KEYPAD_0,
    KEY_F7,          KEYPAD_ASTERIX,KEYPAD_7,        KEYPAD_4,      KEYPAD_1,                NO_KEY,
    KEY_F8,          KEYPAD_MINUS,  KEYPAD_8,        KEYPAD_5,      KEYPAD_2,                KEYPAD_PERIOD,
    KEY_F9,          KEY_MINUS,     KEYPAD_9,        KEYPAD_6,      KEYPAD_3,                KEYPAD_ENTER,
    KEY_F10,         KEY_EQUAL,     KEYPAD_MINUS,    KEYPAD_PLUS,   NO_KEY,                  NO_KEY,
    KEY_F11,         KEY_BACKSPACE, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEYPAD_PLUS,             KEY_MENU,
    KEY_F12,         NO_KEY,        KEY_ENTER,       NO_KEY,        NO_KEY,                  MODIFIERKEY_RIGHT_CTRL,
    KEY_PRINTSCREEN, KEY_INSERT,    KEY_DELETE,      NO_KEY,        NO_KEY,                  KEY_LEFT,
    KEY_SCROLL_LOCK, KEY_HOME,      KEY_END,         NO_KEY,        KEY_UP,                  KEY_DOWN,
    KEY_PAUSE,       KEY_PAGE_UP,   KEY_PAGE_DOWN,   NO_KEY,        NO_KEY,                  KEY_RIGHT
  }
};

char keys[RIGHT_COLS][RIGHT_ROWS] = {
{'A', 'B', 'C', 'D', 'E', 'F'},
{'G', 'H', 'I', 'J', 'K', 'L'},
{'M', 'N', 'O', 'P', 'Q', 'R'},
{'S', 'T', 'U', 'V', 'W', 'X'},
{'Y', 'Z', 'a', 'b', 'c', 'd'},
{'e', 'f', 'g', 'h', 'i', 'j'},
{'k', 'l', 'm', 'n', 'o', 'p'}
};
byte rowPins[RIGHT_ROWS] = {21, 20, 19, 18, 17, 16};
byte colPins[RIGHT_COLS] = { 9, 22, 11, 13, 14, 15, 12};

Keypad kpd = Keypad( makeKeymap(keys), colPins, rowPins, RIGHT_COLS, RIGHT_ROWS );

unsigned long loopCount;
unsigned long startTime;
String msg;
byte currentLayout;

void setup() {
    Serial.begin(9600);
    
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receiveKey);
    
    loopCount = 0;
    startTime = millis();
    msg = "";
    currentLayout = 0;
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
            uint16_t keyChar = layoutsRight[currentLayout][kpd.key[i].kcode];
            
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

void receiveKey(int howMany) {
  int x = Wire.read();    // receive byte as an integer
  uint16_t keyChar;
  if(x < 128)
  {
    keyChar = layoutsLeft[currentLayout][x];
    if(keyChar < LAYOUT_NBR){
      Serial.print("Switching layout to");
      Serial.println(keyChar);
      currentLayout = keyChar;
    }
    else {
      Keyboard.press(keyChar);
      Serial.print("Key ");
      Serial.print(x);
      Serial.println(" PRESSED.");
    }
  }
  else
  {
    keyChar = layoutsLeft[currentLayout][x - 128];
    
    if(keyChar > LAYOUT_NBR){  
      Keyboard.release(keyChar);
      Serial.print("Key ");
      Serial.print(x - 128);
      Serial.println(" RELEASED.");
    }
  }
}
