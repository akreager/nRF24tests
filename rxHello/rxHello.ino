/*
  Arduino Wireless Communication Tutorial
        Example 1 - Receiver Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Serial LCD commands
#define COMMAND 0xFE
#define CLEAR   0x01
#define LINE0   0x80
#define LINE1   0xC0

RF24 radio(49, 53); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  //Communicate with serial LCD at 9600 baud
  Serial2.begin(9600);
  clearLCD();
  delay(100);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    Serial2.write(COMMAND);
    Serial2.write(LINE0);
    Serial2.print(text);
  }
}

void clearLCD(void)
{
  Serial2.write(COMMAND);
  Serial2.write(CLEAR);
}
