
#include <Bounce2.h>
#include "SSD1306Brzo.h"

#define IO_BTN_LEFT   D3
#define IO_BTN_OK     D5
#define IO_BTN_UP     D6 
#define IO_BTN_DOWN   D7
//RGB diode
#define IO_LED_RED    D3
#define IO_LED_GREEN  D4
#define IO_LED_BLUE   D8

#define IO_BUZZER     D10

#define LED_OFF       1
#define LED_ON        0

Bounce btn_left;
Bounce btn_ok;
Bounce btn_up;
Bounce btn_down;

SSD1306Brzo display(0x3C, D1, D2);

void Design0(unsigned int tick, unsigned int mode, bool busy);
void Design1(unsigned int tick, unsigned int mode, bool busy);
void Design2(unsigned int tick, unsigned int mode, bool busy);
void Design3(unsigned int tick, unsigned int mode, bool busy);

typedef void (*DesignCallback)(unsigned int tick, unsigned int mode, bool busy);
DesignCallback callbacks[] = {Design0, Design1, Design2, Design3};

enum mode {
    MODE_OK,
    MODE_WARN,
    MODE_ERROR,
    MODE_BUSY
};

unsigned int design = 0;
unsigned int mode = 0;


void setRGB(char r, char g, char b) {
    analogWrite(IO_LED_RED, r);
    analogWrite(IO_LED_GREEN, g);
    analogWrite(IO_LED_BLUE, b);
}

void updateDisplay(unsigned int design, unsigned int mode, bool busy)
{
    display.clear();
    display.setFont(ArialMT_Plain_16);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0 , 0, "Design:");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(120, 0, String(design));

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 17, "Mode:");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    
    if(mode == MODE_OK)
        display.drawString(120, 17, String("OK"));
    if(mode == MODE_WARN)
        display.drawString(120, 17, String("WARN"));
    if(mode == MODE_ERROR)
        display.drawString(120, 17, String("ERROR"));
    if(mode == MODE_BUSY)
        display.drawString(120, 17, String("BUSY"));

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0 , 34, "Busy:");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if(busy)
        display.drawString(120, 34, String("Yes"));
    else 
        display.drawString(120, 34, String("No"));
    
    display.display();
}
void setup() {
    Serial.begin(115200);
    Serial.println("BIS demo");
    pinMode(BUILTIN_LED, OUTPUT);
    
    pinMode(IO_LED_RED, OUTPUT);
    pinMode(IO_LED_GREEN, OUTPUT);
    pinMode(IO_LED_BLUE, OUTPUT);
    
    //Configure buttons
    btn_left.attach(IO_BTN_LEFT, INPUT_PULLUP);
    btn_up.attach(IO_BTN_UP, INPUT_PULLUP);
    btn_down.attach(IO_BTN_DOWN, INPUT_PULLUP);
    btn_ok.attach(IO_BTN_OK, INPUT_PULLUP);
    
    display.init();

    display.flipScreenVertically();
}

void loop() {   
    btn_left.update();
    btn_up.update();
    btn_down.update();
    btn_ok.update();

    if(btn_ok.fell()) {
        //Pressed OK
        Serial.println("Button OK");
    }
    
    if(btn_left.fell()) {
        //Design switch
        Serial.println("Button left");
        design++;
        if(design >= sizeof(callbacks)/sizeof(callbacks[0])) {
            design=0;
        }
    }
    
    if(btn_up.fell()) {
        Serial.println("Button up");
        mode++;
        if(mode >= 4) 
            mode = 0;
    }
    
    if(btn_down.fell()) {
        Serial.println("Button down");
        mode--;
        if(mode == -1) {
            mode = 3;
        }
    }
    yield();
    
    static bool busy = false;
    static unsigned long lastBusy = 0;
    static unsigned long random_time = random(750);
    if (millis() - lastBusy >= random_time || lastBusy == 0) {
        busy = !busy; //Invert
        random_time = random(750);
        lastBusy = millis();
    }
    
    callbacks[design](millis(), mode, busy);
    
    
    
    if(millis()%100 == 0) {
        updateDisplay(design, mode, busy);
    }
    
} 

void Design0(unsigned int tick, unsigned int mode, bool busy) {
    digitalWrite(BUILTIN_LED, millis()%1000>500);
}


void Design1(unsigned int tick, unsigned int mode, bool busy)
{
    if(mode == MODE_OK ) {
        digitalWrite(BUILTIN_LED, LED_OFF);
    }
    
    if(mode == MODE_WARN) {
        if(tick%1000<10) {
            digitalWrite(BUILTIN_LED, LED_ON);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
        }
    }
    
    if(mode == MODE_ERROR) {
        if(tick%200 < 100) {
            digitalWrite(BUILTIN_LED, LED_ON);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
        }
    }
 
    if(mode == MODE_BUSY) {
       digitalWrite(BUILTIN_LED, LED_ON);
    }
}

void Design2(unsigned int tick, unsigned int mode, bool busy)
{
    if(mode == MODE_OK ) {
        digitalWrite(BUILTIN_LED, LED_OFF);
    }
    
    if(mode == MODE_WARN) {
        if(tick%1000<10) {
            digitalWrite(BUILTIN_LED, LED_ON);
            digitalWrite(IO_BUZZER, 1);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
            digitalWrite(IO_BUZZER, 0);
        }
    }
    
    if(mode == MODE_ERROR) {
        if(tick%200 < 100) {
            digitalWrite(BUILTIN_LED, LED_ON);
            digitalWrite(IO_BUZZER, 1);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
            digitalWrite(IO_BUZZER, 0);
        }
    }
 
    static unsigned int random_time = random(500);
    if(mode == MODE_BUSY) {
        if(busy)
            digitalWrite(BUILTIN_LED, LED_ON);
        else
            digitalWrite(BUILTIN_LED, LED_OFF);
    }
}

void Design3(unsigned int tick, unsigned int mode, bool busy)
{
    if(mode == MODE_OK ) {
        digitalWrite(BUILTIN_LED, LED_OFF);
        setRGB(0,128,0);
    }
    
    if(mode == MODE_WARN) {
        setRGB(128,128,0);
        if(tick%1000<10) {
            digitalWrite(BUILTIN_LED, LED_ON);
            
            digitalWrite(IO_BUZZER, 1);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
            digitalWrite(IO_BUZZER, 0);
        }
    }
    
    if(mode == MODE_ERROR) {
        setRGB(128,0,0);
        if(tick%200 < 100) {
            digitalWrite(BUILTIN_LED, LED_ON);
            digitalWrite(IO_BUZZER, 1);
        } else {
            digitalWrite(BUILTIN_LED, LED_OFF);
            digitalWrite(IO_BUZZER, 0);
        }
    }
 
    if(mode == MODE_BUSY) {
        if(tick%100 < random(100)) {
            setRGB(0,128,0);
        } else {
            setRGB(0,255,0);
        } 
       digitalWrite(BUILTIN_LED, LED_ON);
    }
}