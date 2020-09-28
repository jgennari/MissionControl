/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_I2CDevice.h"

Adafruit_7segment matrix[4];
uint8_t lights[8];

#define GREEN_BUTTON_LED 4
#define RED_BUTTON_LED 5
#define YELLOW_BUTTON_LED 10
#define BLUE_BUTTON_LED 12

#define GREEN_BUTTON 3
#define RED_BUTTON 6
#define YELLOW_BUTTON 9
#define BLUE_BUTTON 11

#define DEFAULT_TIME 0
#define START_TIME 1
#define REMAINING_TIME 2
#define RUNNING 3

// yo/gy/2/or/br

long randnum;
uint16_t blinkcounter = 0;
bool drawDots = false;
long timers[4][4];
uint8_t buttons_led[4];
uint8_t buttons[4];

void set_digits(uint8_t i, uint8_t digit)
{
  matrix[i].writeDigitNum(0, digit);
  matrix[i].writeDigitNum(1, digit);
  matrix[i].writeDigitNum(3, digit);
  matrix[i].writeDigitNum(4, digit);
  matrix[i].writeDisplay();
}

void test_digits()
{
  for (uint8_t digit = 0; digit < 10; digit++)
  {
    for (uint8_t display = 0; display < 4; display++)
    {
      set_digits(display, digit);
    }
    delay(100);
  }
}

void reset_timer(uint8_t i, long timer[4])
{
  long length = 0;
  if (i == 0)
    length = 600000;
  else if (i == 1)
    length = 300000;
  else if (i == 2)
    length = 60000;
  else if (i == 3)
    length = 30000;
  
  timer[DEFAULT_TIME] = length;
  timer[START_TIME] = 0;
  timer[REMAINING_TIME] = length;
  timer[RUNNING] = 0;
}

void setup()
{
  Serial.begin(115200);

  for (uint8_t i = 0; i < 4; i++)
  {
    matrix[i] = Adafruit_7segment();
    if (i == 2)
      matrix[i].begin(115);
    else if (i == 3)
      matrix[i].begin(114);
    else
      matrix[i].begin(0x70 + i);

    matrix[i].clear();
    matrix[i].setBrightness(5);
    matrix[i].writeDisplay();
  }

  test_digits();

  // set default timer length
  reset_timer(0, timers[0]);
  reset_timer(1, timers[1]);
  reset_timer(2, timers[2]);
  reset_timer(3, timers[3]);

  buttons_led[0] = BLUE_BUTTON_LED;
  buttons_led[1] = RED_BUTTON_LED;
  buttons_led[2] = YELLOW_BUTTON_LED;
  buttons_led[3] = GREEN_BUTTON_LED;

  buttons[0] = BLUE_BUTTON;
  buttons[1] = RED_BUTTON;
  buttons[2] = YELLOW_BUTTON;
  buttons[3] = GREEN_BUTTON;

  for (auto &&i : buttons_led)
    pinMode(i, OUTPUT);

  for (auto &&i : buttons)
    pinMode(i, INPUT_PULLUP);
}

void process_timer(uint8_t i, long timer[4])
{
  bool running = timer[RUNNING] == 1 ? true : false;
  if (blinkcounter < 1000)
  {
    if (running) // if timer is running, turn on
      digitalWrite(buttons_led[i], HIGH); 
    else if (!running && timer[REMAINING_TIME] > 0) // if not running and time left then blink
      digitalWrite(buttons_led[i], LOW); // blink
    else
      digitalWrite(buttons_led[i], LOW); // off

    drawDots = !running;
  }
  else if (blinkcounter < 2000)
  {
    if (running) // if timer is running, turn on
      digitalWrite(buttons_led[i], HIGH); 
    else if (!running && timer[REMAINING_TIME] > 0) // if not running and time left then blink
      digitalWrite(buttons_led[i], HIGH); // blink
    else
      digitalWrite(buttons_led[i], LOW); // off
  
    drawDots = true;
  }
  else
  {
    blinkcounter = 0;
  }

  long remaining = timer[REMAINING_TIME];

  if (running)
    remaining = timer[REMAINING_TIME] - (millis() - timer[START_TIME]);
  
  long remaining_sec = ((remaining / 1000) % 60);
  long remaining_min = ((remaining / 1000) / 60);

  if (remaining > 0)
  {
    int displayValue = remaining_min * 100 + remaining_sec;
    matrix[i].print(displayValue);
  }
  else
  {
    timer[REMAINING_TIME] = 0;
    timer[RUNNING] = 0;
  }

  matrix[i].drawColon(drawDots);
  matrix[i].writeDisplay();
}

void process_button(uint8_t i, long timer[4])
{
  if (timer[RUNNING] == 0 && digitalRead(buttons[i]) == LOW)
  {
    if (timer[REMAINING_TIME] == 0)
      reset_timer(i, timer);

    timer[RUNNING] = 1;
    timer[START_TIME] = millis();
    delay(250);
  }
  else if (timer[RUNNING] == 1 && digitalRead(buttons[i]) == LOW)
  {
    timer[REMAINING_TIME] = timer[REMAINING_TIME] - (millis() - timer[START_TIME]);
    timer[RUNNING] = 0;
    delay(250);
  }
}

void loop()
{
  blinkcounter += 50;

  for (uint8_t i = 0; i < 4; i++)
  {
    process_button(i, timers[i]);
    process_timer(i, timers[i]);
  }

  delay(10);
}