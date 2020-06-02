/*
  Optimized fork of nRF24L01 for Arduino & Raspberry Pi/Linux Devices:
  TMRh20/RF24, https://github.com/tmrh20/RF24/
  nRF24/RF24 is licensed under the
  GNU General Public License v2.0

  Based on:
  https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  https://howtomechatronics.com/projects/diy-arduino-rc-transmitter/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Serial LCD commands
#define COMMAND 0xFE
#define CLEAR   0x01
#define LINE0   0x80
#define LINE1   0xC0

//nRF24 pins hardware +SPI
#define CE_PIN 49
#define CSN_PIN 53
RF24 radio(CE_PIN, CSN_PIN);

const byte nRFaddress[6] = "00001";

unsigned int rawBattery = 200;
unsigned int rawTemp1 = 220;
unsigned int rawTemp2 = 220;
int rawCurrentMotor1 = 321;
int rawCurrentMotor2 = 456;
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

int readInterval = 0;
int readThreshold = 10;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct dataPackage {
  byte throttle;
  byte steering;
};
dataPackage data; //Create a variable with the above structure

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

  //Communicate with serial LCD at 9600 baud
  Serial2.begin(9600);
  clearLCD();
  delay(100);

  //set default acknowledge data
  ack.highBatt = highByte(rawBattery);
  ack.lowBatt = lowByte(rawBattery);
  ack.highTemp1 = highByte(rawTemp1);
  ack.lowTemp1 = lowByte(rawTemp1);
  ack.highTemp2 = highByte(rawTemp2);
  ack.lowTemp2 = lowByte(rawTemp2);
  ack.highIM1 = highByte(rawCurrentMotor1);
  ack.lowIM1 = lowByte(rawCurrentMotor1);
  ack.highIM2 = highByte(rawCurrentMotor2);
  ack.lowIM2 = lowByte(rawCurrentMotor2);

  //start wireless connection
  radio.begin();
  radio.openReadingPipe(0, nRFaddress);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.writeAckPayload(0, &ack, sizeof(ackPackage));
  radio.startListening();
  resetData();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(dataPackage));
    lastReceiveTime = millis();
    radio.writeAckPayload(0, &ack, sizeof(ackPackage));
    if (readInterval >= readThreshold) {
      updateACK();
      readInterval = 0;
    }
  }

  //verify connection is alive
  currentTime = millis();
  if (currentTime - lastReceiveTime > 1000 ) {
    //reset to neutral if connection lost
    resetData();
  }

  /*int m1Current = (ack.highIM1 << 8) + ack.lowIM1;

    //display data
    Serial2.write(COMMAND);
    Serial2.write(LINE0);
    Serial2.print(ack.highIM1);
    Serial2.print("   ");
    Serial2.write(COMMAND);
    Serial2.write(LINE0 + 8);
    Serial2.print(readInterval);
    Serial2.print("   ");
    Serial2.write(COMMAND);
    Serial2.write(LINE1);
    Serial2.print(ack.lowIM1);
    Serial2.print("   ");
    Serial2.write(COMMAND);
    Serial2.write(LINE1 + 8);
    Serial2.print(m1Current);
    Serial2.print("   ");*/

  //display recevied data
  Serial2.write(COMMAND);
  Serial2.write(LINE0);
  Serial2.print("Throttle: ");
  Serial2.print(data.throttle);
  Serial2.print("   ");
  Serial2.write(COMMAND);
  Serial2.write(LINE1);
  Serial2.print("Steering: ");
  Serial2.print(data.steering);
  Serial2.print("   ");


  readInterval++;
}

void clearLCD(void) {
  Serial2.write(COMMAND);
  Serial2.write(CLEAR);
}

void resetData() {
  //Reset the values when there is no radio connection
  data.throttle = 64;
  data.steering = 64;
}

void updateACK() {
  //make arbitray changes to the data
  if (rawBattery > 150) {
    rawBattery--;
  }
  else {
    rawBattery = 200;
  }

  if (rawTemp1 < 450) {
    rawTemp1++;
  }
  else {
    rawTemp1 = 220;
  }

  if (rawTemp2 < 451) {
    rawTemp2++;
  }
  else {
    rawTemp2 = 220;
  }

  if (rawCurrentMotor1 < 700) {
    rawCurrentMotor1++;
  }
  else {
    rawCurrentMotor1 = 0;
  }

  if (rawCurrentMotor2 < 700) {
    rawCurrentMotor2++;
  }
  else {
    rawCurrentMotor2 = 0;
  }

  ack.highBatt = highByte(rawBattery);
  ack.lowBatt = lowByte(rawBattery);
  ack.highTemp1 = highByte(rawTemp1);
  ack.lowTemp1 = lowByte(rawTemp1);
  ack.highTemp2 = highByte(rawTemp2);
  ack.lowTemp2 = lowByte(rawTemp2);
  ack.highIM1 = highByte(rawCurrentMotor1);
  ack.lowIM1 = lowByte(rawCurrentMotor1);
  ack.highIM2 = highByte(rawCurrentMotor2);
  ack.lowIM2 = lowByte(rawCurrentMotor2);
}
