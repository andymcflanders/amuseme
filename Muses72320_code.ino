#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Muses72320.h"

#define ENCODER_CLK_PIN 2
#define ENCODER_DT_PIN 3
#define ENCODER_SW_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const uint8_t MUSES_ADDRESS = 0;
const int LATCH_PIN = 10;

Muses72320 Muses(MUSES_ADDRESS, LATCH_PIN); // Adds Muse chip with address 000, and Chip Select/Latch connected on arduino pin 10
int CurrentVolume = -200; // audio level goes from [-111.5, 0.0] dB, input goes from -223 to 0.
volatile bool updated = false;
int lastClkState = HIGH;

void setup() {
  Muses.begin();
    // Initialize muses (SPI, pin modes)...
  Muses.begin();

  // Muses initially starts in a muted state, set a volume to enable sound.
  Muses.setVolume(CurrentVolume);

  // These are the default states and could be removed...
  Muses.setAttenuationLink(true); // L/R attentuation channels are independent controlled.
  
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), readEncoder, CHANGE);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  drawDisplay();
}

void readEncoder() {
  int clkState = digitalRead(ENCODER_CLK_PIN);
  int dtState = digitalRead(ENCODER_DT_PIN);
  if (clkState != lastClkState) {
    if (dtState != clkState) CurrentVolume++;
    else CurrentVolume--;
    CurrentVolume = constrain(CurrentVolume, -223, 0);
    updated = true;
    lastClkState = clkState;
  }
}



//Draws Volume to SSD1306 OLED
void drawDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("VOLUM");
  display.setCursor(0, 30);
  display.println(CurrentVolume);
  display.display();
}

//Main loop, checks for new input from encoder. Updates volume accordingly
void loop() {
  if (updated) {
    Muses.setVolume(CurrentVolume);
    drawDisplay();
    updated = false;
  }
}
