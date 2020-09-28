#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LedControl_SW_SPI.h"

#define NEOPIXEL_PIN 3
#define MATRIX_CLK 8
#define MATRIX_CS 9
#define MATRIX_DATA 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
LedControl_SW_SPI lc = LedControl_SW_SPI();
unsigned long delaytime = 100;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
  for (int j = 0; j < 256; j++)
  { // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 4)
      {
        strip.setPixelColor(i + q, Wheel((i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 4)
      {
        strip.setPixelColor(i + q, 0); //turn every third pixel off
      }
    }
  }
}

void setup()
{
  delay(200);
  lc.begin(MATRIX_DATA, MATRIX_CLK, MATRIX_CS, 4);

  for (size_t i = 0; i < 5; i++)
  {
    lc.shutdown(i, false);
    delay(50);
    lc.setIntensity(i, 1);
    delay(50);
    lc.clearDisplay(i);
    delay(50);
  }

  strip.begin();
  strip.clear();
  strip.setBrightness(1);
  strip.show();

  delay(2000);
}

long blink = 0;
void loop()
{
  
    for (size_t d = 0; d < 5; d++)
    {
      lc.clearDisplay(d);
      for (size_t i = 0; i < 5; i++)
      {
        uint8_t rand_x = random(0, 7);
        uint8_t rand_y = random(0, 7);
        lc.setLed(d, rand_x, rand_y, true);
      }
    }
    delay(250);

    strip.clear();
    strip.show();
    for (size_t i = 0; i < 15; i++)
    {
      uint8_t pixel = random(0,63);
      uint8_t r = random(40,200);
      uint8_t g = random(40,200);
      uint8_t b = random(40,200);
      strip.setPixelColor(pixel, 255, 0, 0);
    }
    strip.show();
    delay(250);
    

  //theaterChaseRainbow(100);
}
