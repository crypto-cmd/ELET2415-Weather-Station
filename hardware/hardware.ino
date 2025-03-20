// ##################################################################################################################
// ##                                      ELET2415 DATA ACQUISITION SYSTEM CODE                                   ##
// ##                                                                                                              ##
// ##################################################################################################################

// IMPORT ALL REQUIRED LIBRARIES
#include <rom/rtc.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>

#ifndef _WIFI_H
#include <WiFi.h>
#include <HTTPClient.h>
#endif

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

#define DRY 2900 // The experimental value of DRY AIR
#define WET 1200 // The experimental value of 100% WET

// IMPORT FONTS FOR TFT DISPLAYM
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// MQTT CLIENT CONFIG
static const char *pubtopic = "620164974";                      // Add your ID number here
static const char *subtopic[] = {"620164974_sub", "/elet2415"}; // Array of Topics(Strings) to subscribe to
static const char *mqtt_server = "www.yanacreations.com";       // Broker IP address or Domain name as a String
static uint16_t mqtt_port = 1883;

// WIFI CREDENTIALS
const char *ssid = "MonaConnect"; // Add your Wi-Fi ssid
const char *password = "";      // Add your Wi-Fi password

// TASK HANDLES
TaskHandle_t xMQTT_Connect = NULL;
TaskHandle_t xNTPHandle = NULL;
TaskHandle_t xLOOPHandle = NULL;
TaskHandle_t xUpdateHandle = NULL;
TaskHandle_t xButtonCheckeHandle = NULL;

// FUNCTION DECLARATION
void checkHEAP(const char *Name); // RETURN REMAINING HEAP SIZE FOR A TASK
void initMQTT(void);              // CONFIG AND INITIALIZE MQTT PROTOCOL
unsigned long getTimeStamp(void); // GET 10 DIGIT TIMESTAMP FOR CURRENT TIME
void callback(char *topic, byte *payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload); // PUBLISH MQTT MESSAGE(PAYLOAD) TO A TOPIC
void vButtonCheck(void *pvParameters);
void vUpdate(void *pvParameters);

// ############### IMPORT HEADER FILES ##################
#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

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

    /* Configure all others here */

    initialize(); // INIT WIFI, MQTT & NTP
                  // vButtonCheckFunction(); // UNCOMMENT IF USING BUTTONS THEN ADD LOGIC FOR INTERFACING WITH BUTTONS IN THE vButtonCheck FUNCTION
}

void loop()
{
    SensorDataPoint data = readSensors();
    displayReadings(data);
    printReadings(data);

    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

// Weather Station specific functions
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
    // Clamp the soil value to between DRY and WET
    if (soil > DRY)
    {
        soil = DRY;
    }
    else if (soil < WET)
    {
        soil = WET;
    }
    float soilPercentage = map(soil, WET, DRY, 100, 0);

    SensorDataPoint data = {temp, humidity, pressure, altitude, moisture : soilPercentage, heatIndex : hi};
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
    tft.println(" %");           // Unit

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

// ####################################################################
// #                          UTIL FUNCTIONS                          #
// ####################################################################

void vUpdate(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    for (;;)
    {
        // Task code goes here.
        // PUBLISH to topic every second.
        JsonDocument doc; // Create JSon object
        char message[1100] = {0};

        // Perform reading
        SensorDataPoint data = readSensors();

        // Add key:value pairs to JSon object
        doc["id"] = "620164974"; // Change to your student ID number
        doc["timestamp"] = getTimeStamp();
        doc["temperature"] = data.temp;
        doc["humidity"] = data.humidity;
        doc["pressure"] = data.pressure;
        doc["altitude"] = data.altitude;
        doc["moisture"] = data.moisture;
        doc["heatIndex"] = data.heatIndex;

        serializeJson(doc, message); // Seralize / Covert JSon object to JSon string and store in char* array

        if (mqtt.connected())
        {
            publish(pubtopic, message);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

unsigned long getTimeStamp(void)
{
    // RETURNS 10 DIGIT TIMESTAMP REPRESENTING CURRENT TIME
    time_t now;
    time(&now); // Retrieve time[Timestamp] from system and save to &now variable
    return now;
}

bool publish(const char *topic, const char *payload)
{
    bool res = false;
    try
    {
        res = mqtt.publish(topic, payload);
        // Serial.printf("\nres : %d\n",res);
        if (!res)
        {
            res = false;
            throw false;
        }
    }
    catch (...)
    {
        Serial.printf("\nError (%d) >> Unable to publish message\n", res);
    }
    return res;
}

void callback(char *topic, byte *payload, unsigned int length)
{
    // ############## MQTT CALLBACK  ######################################
    // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO

    Serial.printf("\nMessage received : ( topic: %s ) \n", topic);
    char *received = new char[length + 1]{0};

    for (int i = 0; i < length; i++)
    {
        received[i] = (char)payload[i];
    }

    // PRINT RECEIVED MESSAGE
    Serial.printf("Payload : %s \n", received);

    // CONVERT MESSAGE TO JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, received);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
}