#include <Adafruit_NeoPixel.h>

#define PIN_LEDS          32
#define PIN_BRIGHTNESS    34
#define PIN_SATURATION    35
#define PIN_PERIOD        33

#define ANALOG_SAMPLES    32
#define ANALOG_DEADZONE   5 
#define NUMPIXELS         200
#define N_STEPS           256
#define T_PERIOD_MAX      10
#define T_STEP            (5 / 1024)
#define MAX_HUE_GRADIENT  (65535.0 / 2 / N_STEPS)

// #define SATURATION        150
// #define BRIGHTNESS        128

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_LEDS, NEO_RGB + NEO_KHZ800);

float current_hue[NUMPIXELS];
float gradient_hue[NUMPIXELS];

int old_saturation = 150;
int old_brightness = 128;
int old_step_delay = 0;

int t0, t1;


void setup() {

  Serial.begin(115200);

  pixels.begin();

  for(int i=0; i<NUMPIXELS; i++) {
    current_hue[i] = float(random(0, 65536));
  }
}

void loop() {

  uint16_t new_hue;
  uint16_t raw_hue;
  uint32_t hue;
  
  for(int i=0; i<NUMPIXELS; i++) {

    gradient_hue[i] = random(-MAX_HUE_GRADIENT, MAX_HUE_GRADIENT);
  }

  Serial.print("saturation = ");
  Serial.print(old_saturation);
  Serial.print(", brightness = ");
  Serial.print(old_brightness);
  Serial.print(", step_delay = ");
  Serial.println(old_step_delay);
  Serial.println(gradient_hue[0]);
  Serial.println(current_hue[0]);

  Serial.print("dt0 = ");
  Serial.println(t1 - t0);

  for(int i=0; i<N_STEPS; i++) {

    int saturation = 150;
    int brightness = 128;
    int step_delay = 0;

    for (int i=0; i<ANALOG_SAMPLES; i++) {
      saturation += analogRead(PIN_SATURATION) >> 4;
      brightness += analogRead(PIN_BRIGHTNESS) >> 4;
      step_delay += 1000.0 * analogRead(PIN_PERIOD) / 4095 * T_PERIOD_MAX / N_STEPS;
    }

    saturation = saturation / ANALOG_SAMPLES;
    brightness = brightness / ANALOG_SAMPLES;
    step_delay = step_delay / ANALOG_SAMPLES;

    if (abs(saturation - old_saturation) > ANALOG_DEADZONE) {
      old_saturation = saturation;
    } else {
      saturation = old_saturation;
    }

    if (abs(brightness - old_brightness) > ANALOG_DEADZONE) {
      old_brightness = brightness;
    } else {
      brightness = old_brightness;
    }

    if (abs(step_delay - old_step_delay) > ANALOG_DEADZONE) {
      old_step_delay = step_delay;
    } else {
      step_delay = old_step_delay;
    }
    
    for(int j=0; j<NUMPIXELS; j++) {

      current_hue[j] = current_hue[j] + gradient_hue[j];

      if (current_hue[j] < 0.0) {
        current_hue[j] += 65535.0;
      } else if (current_hue[j] > 65535.0) {
        current_hue[j] -= 65535.0;
      }

      raw_hue = uint16_t(constrain(current_hue[j], 0, 65535));
      hue = pixels.gamma32(pixels.ColorHSV(raw_hue, saturation, brightness));

      pixels.setPixelColor(j, hue);    
    }

    delay(step_delay);

    t0 = micros();
    pixels.show();
    t1 = micros(); 
  }
}