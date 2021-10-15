#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_TEXT_SIZE 2
#define SCREEN_PADDING 2
#define SCREEN_LINE 20 // OLED text internal size is 8 this should be 8*Text_Size+padding*2

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

#define NEO_LED_PIN 6
#define NEO_LED_NUM 1
#define NEO_LED_MAX 128

Adafruit_NeoPixel pixels(NEO_LED_NUM, NEO_LED_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayLedClear() {
  pixels.clear();
  pixels.show();
}

void displayLedColor(int red, int green, int blue, int time) {
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.show();
  delay(time);
}

void displayLedPulse(int rate) {
  float brightness = 0;
  // ramp up
  for (int i = 0; i <= rate; i++) {
    brightness = cos((i / rate) * M_PI * 2);
    brightness = (brightness * -0.5) + 0.5; 
    brightness = pow(brightness, 2.0);
    pixels.setBrightness(brightness * NEO_LED_MAX);
    pixels.show();
    delay(1);
  }
}

void displayLedBlinking(int red, int green, int blue, int rate, int times) {
  displayLedColor(red, green, blue, rate);
  for (int i = 0; i <= times; i++) {
    displayLedPulse(rate);
  }
}

void displayClearLine(int line) {
  display.fillRect(0, line*SCREEN_LINE, 128, SCREEN_LINE, SSD1306_BLACK);
  display.display();
}

void displayProgress(float percentage) {
  int width = 128 * (0.01 * constrain(percentage, 0 ,100));
  display.fillRect(0, SCREEN_LINE*3, width, 4, SSD1306_WHITE);
  display.fillRect(width, SCREEN_LINE*3, 128-width, 4, SSD1306_BLACK);
  display.display();
}

void displayLineInfo(int line, const char info[], int drawMode, bool scroll) {
  // Put something here to make sure we add white spaces to the end
  display.setTextSize(SCREEN_TEXT_SIZE);
  display.setTextWrap(false);
  if (drawMode > 0) {
    display.fillRect(0, line*SCREEN_LINE, 128, SCREEN_LINE, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  }
  else {
    display.fillRect(0, line*SCREEN_LINE, 128, SCREEN_LINE, SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE); // Draw white text
  }
  
  display.setCursor(SCREEN_PADDING, (line*SCREEN_LINE)+SCREEN_PADDING); // Set the top-left corner
  display.println(info);
  
  if (scroll) {
    display.startscrollleft(line*SCREEN_LINE, (line+1)*SCREEN_LINE);
  }
  
  display.display();
}

void displayLineInfo(int line, const String &info, int drawMode, bool scroll) {
  displayLineInfo(line, info.c_str(), drawMode, scroll);
}

void displayLineInfo(int line, const String &info, int drawMode) {
  displayLineInfo(line, info.c_str(), drawMode, false);
}

void displayLineInfo(int line, const String &info) {
  displayLineInfo(line, info, 0);
}

void displayScrollLine(int line) {
  display.startscrollleft(line*SCREEN_LINE, (line+1)*SCREEN_LINE);
}

void displayInitialize() {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(NEO_LED_MAX);
  displayLedColor(128, 0, 0, 250);
  displayLedColor(0, 128, 0, 250);
  displayLedColor(0, 0, 128, 250);
  displayLedClear();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    displayLedBlinking(128, 0, 0, 250, 25);
    for(;;); // Don't proceed, loop forever
  }

  // Flip screen....
  //display.setRotation(2);
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  displayLedBlinking(0, 128, 128, 250, 8);

  // Clear the buffer
  display.clearDisplay();
  display.display();
}