//Serial LCD commands
#define COMMAND 0xFE
#define CLEAR   0x01
#define LINE0   0x80
#define LINE1   0xC0

unsigned int rawBattery = 0;
byte highBatt;
byte lowBatt;
unsigned int combBatt = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //Communicate with serial LCD at 9600 baud
  Serial2.begin(9600);
  clearLCD();
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:
  clearLCD();
  highBatt = highByte(rawBattery);
  lowBatt = lowByte(rawBattery);
  combBatt = (highBatt << 8) + lowBatt;
  int diff = combBatt - rawBattery;
  Serial2.write(COMMAND);
  Serial2.write(LINE0);
  Serial2.print(rawBattery);
  Serial2.write(COMMAND);
  Serial2.write(LINE0 + 8);
  Serial2.print(combBatt);
  Serial2.write(COMMAND);
  Serial2.write(LINE0 + 15);
  Serial2.print(diff);
  Serial2.write(COMMAND);
  Serial2.write(LINE1);
  Serial2.print(highBatt);
  Serial2.write(COMMAND);
  Serial2.write(LINE1 + 8);
  Serial2.print(lowBatt);

  if (combBatt != rawBattery) {
    while (1) {
      digitalWrite(13, HIGH);
    }
  }
  
  rawBattery++;
  delay(100);
}

void clearLCD(void)
{
  Serial2.write(COMMAND);
  Serial2.write(CLEAR);
}
