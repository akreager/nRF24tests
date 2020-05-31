/*
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

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte throttle;
  byte steeting;
};

Data_Package data; //Create a variable with the above structure


void setup() {
  Serial.begin(9600);

  //Communicate with serial LCD at 9600 baud
  Serial2.begin(9600);
  clearLCD();
  delay(100);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  resetData();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }

  //display data
  Serial2.write(COMMAND);
  Serial2.write(LINE0);
  Serial2.print("Throttle: ");
  Serial2.print(data.throttle);
  Serial2.print("   ");
  Serial2.write(COMMAND);
  Serial2.write(LINE1);
  Serial2.print("Steering: ");
  Serial2.print(data.steeting);
  Serial2.print("   ");
}

void clearLCD(void)
{
  Serial2.write(COMMAND);
  Serial2.write(CLEAR);
}

void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  data.throttle = 64;
  data.steeting = 64;
}
