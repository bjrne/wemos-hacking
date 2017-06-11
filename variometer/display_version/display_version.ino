/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 64x48 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

double groundLevel;
double value;
double currentvalue;
double diff;
int displaydiff;
int counter;
int measurementsPerSecond = 2;
int lift = 0;

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {
  Serial.begin(9600);

  if (!bmp.begin(0x76)) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  groundLevel = bmp.readPressure();
  value  = bmp.readAltitude(groundLevel);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  
  display.setTextSize(5);
  display.setTextColor(WHITE);
 
}


void loop() {
  currentvalue  = bmp.readAltitude(groundLevel);
  diff = currentvalue-value;
  Serial.println(diff);
  //Serial.println(currentvalue);
  if (diff < 0) {
    lift = -1;
  } else {
    lift = 1;
  }
  double temp = (diff + (0.049 * lift)) * 10 * measurementsPerSecond;
  displaydiff = (int) temp;
  displaydiff *= lift;
  
  if (diff > 0.1) {
    showLift(displaydiff);
  } else if (diff < -0.2){
    showSink(displaydiff);
  } else {
    display.clearDisplay();
    display.invertDisplay(false);
    display.display();
  }
  value = currentvalue;
  delay(1000/measurementsPerSecond);
}

void showLift(int rate) {
  display.clearDisplay();
  display.invertDisplay(false);
  display.setCursor(0,0);
  if (rate < 10) {
    display.print(0);
  }
  display.println(rate);
  printComma(26, 36);
  display.display();
}

void showSink(int rate) {
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0,0);
  if (rate < 10) {
    display.print(0);
  }
  display.println(rate);
  printComma(26, 36);
  display.display();
}

void printComma(int startX, int startY) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      display.drawPixel(startX+i, startY+j, WHITE);
    }
  }
}

