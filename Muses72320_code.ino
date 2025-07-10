#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LATCH_PIN 10
//#define ZCEN_PIN 9
#define ENCODER_CLK_PIN 2
#define ENCODER_DT_PIN 3
#define ENCODER_SW_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
volatile uint8_t gain = 64;
volatile bool updated = false;
int lastClkState = HIGH;

void setup() {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV64);//DIV4
  SPI.setDataMode(SPI_MODE2);//MODE0
  //digitalWrite(ZCEN_PIN, LOW);
  SPI.setBitOrder(MSBFIRST);

  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
  //pinMode(ZCEN_PIN, OUTPUT);
  //digitalWrite(ZCEN_PIN, HIGH);

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
    if (dtState != clkState) gain++;
    else gain--;
    gain = constrain(gain, 0, 127);
    updated = true;
    lastClkState = clkState;
  }
}
void sendVolume(uint8_t gainValue) {
  uint16_t word = (0b0000 << 12) | ((gainValue & 0x7F) << 8);
  uint8_t highByte = (word >> 8) & 0xFF;
  uint8_t lowByte = word & 0xFF;
  digitalWrite(LATCH_PIN, LOW);
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  delayMicroseconds(2);
  digitalWrite(LATCH_PIN, HIGH);
//}
//digitalWrite(LATCH_PIN, LOW); // Link L og R volum
//SPI.transfer(highByte (0B10000000));
//SPI.transfer(lowByte (0B01000000));
//delayMicroseconds(2);
//digitalWrite(LATCH_PIN, HIGH);
}
void drawDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("VOLUM");
  display.setCursor(0, 30);
  display.println(gain);
  display.display();
}
void loop() {
  if (updated) {
    sendVolume(gain);
    drawDisplay();
    updated = false;
  }
}

