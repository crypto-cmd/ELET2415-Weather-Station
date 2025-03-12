// ##################################################################################################################
// ##                                      ELET2415 WEATHER STATION HARDWARE CODE                                  ##
// ##                                                                                                              ##
// ##################################################################################################################

// IMPORT ALL REQUIRED LIBRARIES
#include <rom/rtc.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>


// DEFINE VARIABLES
#define soilPin 32
// #define vibrationPin 32

#define DHTPIN 26
#define DHTTYPE DHT11

#define TFT_CS 19
#define TFT_RST 18
#define TFT_DC 5
#define TFT_MOSI 17
#define TFT_SCK 16
#define TFT_MISO 4

#define BOX_WIDTH 50
#define BOX_HEIGHT 50
#define SPACING 5
#define MARGIN 10

// IMPORT FONTS FOR TFT DISPLAYM
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

/* Initialize class objects*/
Adafruit_BMP280 bmp; // I2C
DHT dht(DHTPIN, DHTTYPE);

struct SensorDataPoint
{
  float temp, humidity, pressure, altitude, vibration;
  float moisture;
  float heatIndex;

};
/* Declare your functions below */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

void startSensors();
SensorDataPoint readSensors();
void displayReadings(SensorDataPoint &data);
void printReadings(SensorDataPoint &data);
void startTFT();

void setup()
{
  Serial.begin(115200); // INIT SERIAL

  startSensors();
  startTFT();
}

void loop()
{
  SensorDataPoint data = readSensors();
  displayReadings(data);
  printReadings(data);

  delay(1000);
}

// ####################################################################
// #                          UTIL FUNCTIONS                          #
// ####################################################################
//***** Complete the util functions below ******

void startSensors()
{

  pinMode(soilPin, INPUT);

  dht.begin();
  while (!bmp.begin(0x76))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    delay(1000);
  }
}

SensorDataPoint readSensors()
{
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(1014.5);
  int soil = analogRead(soilPin);

  float hi = dht.computeHeatIndex(false);


  Serial.println(soil);
  float soilPercentage = map(soil, 0, 4095, 100, 0);

  if (soilPercentage > 100) {
    soilPercentage = 100;
  } else if (soilPercentage < 0){
    soilPercentage = 0;
  }
  SensorDataPoint data = {temp, humidity, pressure, altitude, moisture : soilPercentage, heatIndex: hi};
  return data;
}

void displayReadings(SensorDataPoint &data)
{
  // Print labels
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);

  // Define label positions
  int startX = 10;
  int startY = 10;
  int yGap = 50;
  int rectHeight = 20; // Adjust as needed

  // Clear area and Print Temperature
  // tft.fillRect(startX, startY, 300, rectHeight, ILI9341_BLACK);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);

  tft.setCursor(startX, startY);
  tft.print("Temperature: ");
  tft.print(data.temp, 2); // Value rounded to 2 decimal places
  tft.println(" C");       // Unit

  // Clear area and Print Humidity
  // tft.fillRect(startX, startY + yGap, 300, rectHeight, ILI9341_BLACK);
  tft.setCursor(startX, startY + yGap);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

  tft.print("Humidity: ");
  tft.print(data.humidity, 2); // Value rounded to 2 decimal places
  tft.println(" %");           // Unit

  // Clear area and Print Pressure
  // tft.fillRect(startX, startY + 2 * yGap, 300, rectHeight, ILI9341_BLACK);
  tft.setCursor(startX, startY + 2 * yGap);
  tft.setTextColor(ILI9341_PINK, ILI9341_BLACK);

  tft.print("Pressure: ");
  tft.print(data.pressure, 2); // Value rounded to 2 decimal places
  tft.println(" Pa");          // Unit

  // Clear area and Print Altitude
  // tft.fillRect(startX, startY + 3 * yGap, 300, rectHeight, ILI9341_BLACK);
  tft.setCursor(startX, startY + 3 * yGap);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);

  tft.print("Altitude: ");
  tft.print(data.altitude, 2); // Value rounded to 2 decimal places
  tft.println(" m");           // Unit

  // Clear area and Print Soil
  // tft.fillRect(startX, startY + 4 * yGap, 300, rectHeight, ILI9341_BLACK);
  tft.setCursor(startX, startY + 4 * yGap);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);

  tft.print("Soil: ");
  tft.print(data.moisture, 2); // Value rounded to 2 decimal places
  tft.println(" %");       // Unit

  // Clear area and Print Heat Index
  // tft.fillRect(startX, startY + 5 * yGap, 300, rectHeight, ILI9341_BLACK);
  tft.setCursor(startX, startY + 5 * yGap);
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);

  tft.print("Heat Index: ");
  tft.print(data.heatIndex, 2); // Value rounded to 2 decimal places
  tft.println(" C");            // Unit
}
void printReadings(SensorDataPoint &data)
{
  Serial.print("Temperature: ");
  Serial.print(data.temp, 2);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(data.humidity, 2);
  Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(data.pressure, 2);
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(data.altitude, 2);
  Serial.println(" m");

  Serial.print("Soil: ");
  Serial.print(data.moisture);
  Serial.println(" %");

  Serial.print("Heat Index: ");
  Serial.print(data.heatIndex, 2);
  Serial.println(" C");
}
void startTFT()
{
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN);
}
