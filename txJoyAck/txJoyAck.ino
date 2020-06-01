/*
  Optimized fork of nRF24L01 for Arduino & Raspberry Pi/Linux Devices:
  TMRh20/RF24, https://github.com/tmrh20/RF24/
  nRF24/RF24 is licensed under the
  GNU General Public License v2.0

  Based on:
  https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  https://howtomechatronics.com/projects/diy-arduino-rc-transmitter/

  GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

//nRF24 pins hardware +SPI
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);

// The display uses hardware SPI1, plus #6 & #3
#define TFT_CS 6
#define TFT_DC 3
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI1, TFT_DC, TFT_CS);

const byte address[6] = "00001";

unsigned int rawBatt = 0;
unsigned int rawTemp1 = 0;
unsigned int rawTemp2 = 0;
int rawCurrentMotor1 = 0;
int rawCurrentMotor2 = 0;

unsigned int oldBatt;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte throttle;
  byte steeting;
};
Data_Package data; //Create a variable with the above structure

struct ackPackage {
  byte highBatt;
  byte lowBatt;
  byte highTemp1;
  byte lowTemp1;
  byte highTemp2;
  byte lowTemp2;
  byte highIM1;
  byte lowIM1;
  byte highIM2;
  byte lowIM2;
};
ackPackage ack;

void setup() {
  //Serial.begin(9600);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("Batt:");
  tft.println("Temp1:");
  tft.println("Temp2:");
  tft.println("M1 Current:");
  tft.println();
  tft.println("M2 Current: ");
  tft.setTextColor(ILI9341_GREEN);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  //radio.stopListening();
  radio.enableAckPayload();
  radio.setRetries(3, 5);

  //set default data values
  data.throttle = 64;
  data.steeting = 64;

  /*//Set default ack values
    ack.highBatt = 0;
    ack.lowBatt = 0;
    ack.highTemp1 = 0;
    ack.lowTemp1 = 0;
    ack.highTemp2 = 0;
    ack.lowTemp2 = 0;
    ack.highIM1 = 0;
    ack.lowIM1 = 0;
    ack.highIM2 = 0;
    ack.lowIM2 = 0;
  */
}

void loop() {
  // put your main code here, to run repeatedly:
  data.throttle = map(analogRead(A1), 0, 1023, 127, 0);
  data.steeting = map(analogRead(A0), 0, 1023, 0, 127);

  radio.write(&data, sizeof(Data_Package));
  if (radio.isAckPayloadAvailable() ) {
    radio.read(&ack, sizeof(ackPackage));
    /*
      Serial.print(ack.rawBatt);
      Serial.print(",");
      Serial.print(ack.rawTemp1);
      Serial.print(",");
      Serial.print(ack.rawTemp2);
      Serial.print(",");
      Serial.print(ack.rawCurrentMotor1);
      Serial.print(",");
      Serial.println(ack.rawCurrentMotor2);
    */

    //reconstitute bytes and do math
    rawBatt = (ack.highBatt << 8 ) + ack.lowBatt;
    int mainBattery = rawBatt / 10;
    //int temp1 = ack.rawTemp1 / 10;
    //int temp2 = ack.rawTemp2 / 10;
    //int currentM1 = ack.rawCurrentMotor1 / 100;
    //int currentM2 = ack.rawCurrentMotor2 / 100;

    //print battery value
    if (oldBatt != rawBatt) {
      tft.fillRect(110, 0, 120, 24, ILI9341_BLACK);
    }
    tft.setCursor(110, 0);
    //tft.print(ack.rawBatt);
    tft.print(mainBattery);
    tft.print(".");
    tft.print(rawBatt - (mainBattery * 10));
    tft.print("V ");

    /*   //print temp1 value
       tft.setCursor(110, 24);
       tft.print(temp1);
       tft.print(".");
       tft.print(ack.rawTemp1 - (temp1 * 10));
       tft.print((char)247);
       tft.print("C ");

       //prtin temp2 value
       tft.setCursor(110, 48);
       tft.print(temp2);
       tft.print(".");
       tft.print(ack.rawTemp2 - (temp2 * 10));
       tft.print((char)247);
       tft.print("C ");

       //print M1 current value
       tft.setCursor(60, 96);
       tft.print(currentM1);
       tft.print(".");
       tft.print(ack.rawCurrentMotor1 - (currentM1 * 100));
       tft.print("A ");

       //print M2 current value
       tft.setCursor(60, 144);
       tft.print(currentM2);
       tft.print(".");
       tft.print(ack.rawCurrentMotor2 - (currentM2 * 100));
       tft.print("A ");
    */
    oldBatt = rawBatt;
  }
}
