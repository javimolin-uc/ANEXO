

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_16X);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int led = 9;
const int boton = 4;

struct HSV {
  float h;
  float s;
  float v;
};

HSV rgbToHsv(float r, float g, float b) {
  float rNorm = r / 255.0;
  float gNorm = g / 255.0;
  float bNorm = b / 255.0;

  float max = max(rNorm, max(gNorm, bNorm));
  float min = min(rNorm, min(gNorm, bNorm));
  float delta = max - min;

  float h = 0;
  float s = 0;
  float v = max;

  if (delta != 0) {
    s = delta / max;

    if (max == rNorm) {
      h = 60 * fmod(((gNorm - bNorm) / delta), 6);
    } else if (max == gNorm) {
      h = 60 * (((bNorm - rNorm) / delta) + 2);
    } else if (max == bNorm) {
      h = 60 * (((rNorm - gNorm) / delta) + 4);
    }
  }

  if (h < 0) {
    h += 360;
  }

  HSV hsv = { h, s, v };
  return hsv;
}


String getColorFromHsv(HSV hsv) {
  if (hsv.v < 0.2) {
    return "Negro";
  } else if (hsv.v > 0.8 && hsv.s < 0.2) {
    return "Blanco";
  } else if (hsv.v > 0.5 && hsv.s < 0.2) {
    return "GrisClaro";
  } else if (hsv.v > 0.2 && hsv.s < 0.2) {
    return "GrisOscuro";
  } else {
    float h = hsv.h;
    if (h >= 0 && h < 15) {
      return "Rojo";
    } else if (h >= 15 && h < 45) {
      return "Naranja";
    } else if (h >= 45 && h < 75) {
      return "Amarillo";
    } else if (h >= 75 && h < 165) {
      return "Verde";
    } else if (h >= 165 && h < 195) {
      return "Cian";
    } else if (h >= 195 && h < 255) {
      return "Azul";
    } else if (h >= 255 && h < 285) {
      return "Morado";
    } else if (h >= 285 && h < 315) {
      return "Magenta";
    } else if (h >= 315 && h < 345) {
      return "Rosado";
    } else if (h >= 345 && h < 360) {
      return "Rojo";
    } else {
      return "?????";
    }
  }
}

HSV color_hsv;
String color_str;

void setup() {
  Serial.begin(9600);

  pinMode(led, OUTPUT);
  pinMode(boton, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.display();
  display.setTextColor(SSD1306_WHITE);

  if (tcs.begin()) {
    Serial.println("Sensor encontrado");
  } else {
    Serial.println("No se encontró el TCS3472, revisa las conexiones");
    while (1)
      ;
  }

  display.setTextSize(3);
  display.setCursor(0, 0);
  display.print(F("COLORIS"));
  display.display();
  delay(5000);
  display.clearDisplay();
  display.display();
}

unsigned long previousMillis_sensor = 0;
const long interval_sensor = 2000;
bool state = false;
unsigned long buttonPressTime = 0;  // Tiempo en que se presionó el botón
bool buttonPressed = false;         // Estado de si el botón está presionado

void loop() {
  uint16_t red, green, blue, clear, lux;
  float r, g, b;
  unsigned long currentMillis = millis();


  int buttonState = digitalRead(boton);


  if (buttonState == LOW) {
    if (!buttonPressed) {
      buttonPressTime = millis();
      buttonPressed = true;
    } else {
      if (millis() - buttonPressTime >= 2000) {
        state = !state;
        buttonPressed = false;
        if (state) {
          digitalWrite(led, HIGH);
        } else {
          digitalWrite(led, LOW);
        }
        display.clearDisplay();
        display.display();
      }
    }
  } else {
    buttonPressed = false;
  }

  if (state) {
    if (currentMillis - previousMillis_sensor >= interval_sensor) {
      // Pantalla se actualiza cada 2 segundos
      previousMillis_sensor = currentMillis;

      tcs.getRGB(&r, &g, &b);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println(F("color detectado:"));
      display.setCursor(0, 10);
      display.setTextSize(2);
      color_hsv = rgbToHsv(r, g, b);
      color_str = getColorFromHsv(color_hsv);
      display.println(color_str);
      // display.print(color_hsv.h);
      // display.print(F(";"));
      display.display();
    }
  }
}
