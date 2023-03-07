#include <ATTinyCore.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN    9
#define LED_COUNT 19

#define STATUS_LED_PIN 11

#define PIR_PIN 12
#define BUTTON_PIN 13

#define MILLIS_TIMEOUT 90000

#define TARGET_BRIGHTNESS 80
#define RAMP_MILLIS 1000

#define adjust(color, target) ((float)color / 100 * target)
#define adjust_max(color) (adjust(color, TARGET_BRIGHTNESS))

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel status_strip(1, STATUS_LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long movement_detected_at_ms = 0;
bool eternal_on = false;
bool buttonLatch = false;

void setup() {
  strip.begin();
  strip.setBrightness(100);
  strip.show();

  status_strip.begin();
  status_strip.show();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
}

bool ledsAreOn = false;
void ledsOn(byte r, byte g, byte b) {
  if (!ledsAreOn) {
    for (int i = 1; i <= TARGET_BRIGHTNESS; i++) {
      for (int pixel = 0; pixel < LED_COUNT; pixel++) {
        strip.setPixelColor(pixel, adjust(r, i), adjust(g, i), adjust(b, i));
      }
      
      strip.show();
      delay(RAMP_MILLIS / TARGET_BRIGHTNESS);
    }
  }

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, adjust_max(r), adjust_max(g), adjust_max(b));
  }
  strip.show();

  ledsAreOn = true;
}

void statusLedOn(byte r, byte g, byte b) {
  status_strip.setPixelColor(0, r / 10, g / 10, b / 10);  
  status_strip.show();
}

void ledsOff() {
  strip.clear();
  strip.show();
  ledsAreOn = false;
}

void statusLedOff() {
  status_strip.clear();
  status_strip.show();
}

void loop() {
  const unsigned long now = millis();

  if (digitalRead(BUTTON_PIN)) {
    buttonLatch = !buttonLatch;

    statusLedOn(0, 0,255);

    while (true) {
      if (!digitalRead(BUTTON_PIN)) break;
      delay(50);
    }
    
    statusLedOff();
  }

  if (buttonLatch) {
    statusLedOn(255, 0, 0);
    ledsOn(255, 65, 0);
    delay(50);
    return;
  }

  statusLedOff();

  if (digitalRead(PIR_PIN) > 0) {
    movement_detected_at_ms = now;
  } 

  unsigned long time_elapsed = now - movement_detected_at_ms;

  // handle millis overflow
  if (time_elapsed < 0) {
    time_elapsed = 4294967295 - movement_detected_at_ms + now;
  }

  if (time_elapsed <= MILLIS_TIMEOUT) {
    ledsOn(255, 65, 0);
  } else {
    ledsOff();
  }

  delay(50);
}
