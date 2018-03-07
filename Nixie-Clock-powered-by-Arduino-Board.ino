//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//

#include "DS3231.h"

int SER_Pin = 11;   //pin 14 on the 75HC595 (Data Pin connected to DS of 74HC595)
int RCLK_Pin = 8;  //pin 12 on the 75HC595 (Latch Pin connected to ST_CP of 74HC595)
int SRCLK_Pin = 12; //pin 11 on the 75HC595 (Clock Pin connected to SH_CP of 74HC595)

//How many of the shift registers
#define number_of_74hc595s 3

//The miltiple by 8 is static, do not touch
#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // Initialize 74HC595 pin's
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  //reset all register pins
  clearRegisters();
  writeRegisters();

  // Initialize the rtc object
  rtc.begin();

  // The following lines can be uncommented to set the date and time
  // rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  // rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  // rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
}

void loop()
{
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");

  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());

  // Wait one second before repeating :)
  delay (1000);
}


//set all register pins to LOW
void clearRegisters() {
  for (int i = numOfRegisterPins - 1; i >=  0; i--) {
    registers[i] = LOW;
  }
  writeRegisters();
}

//Set and display/write registers
void writeRegisters() {
  digitalWrite(RCLK_Pin, LOW);
  for (int i = numOfRegisterPins - 1; i >=  0; i--) {
    digitalWrite(SRCLK_Pin, LOW);
    int val = registers[i];
    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);
}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value) {
  registers[index] = value;
}

int numberNixieBinTab[10][4] = {
  {LOW, LOW, LOW, LOW}, // 0  {0,0,0,0},
  {LOW, LOW, LOW, HIGH}, // 1  {0,0,0,1},
  {LOW, LOW, HIGH, LOW}, // 2  {0,0,1,0},
  {LOW, LOW, HIGH, HIGH}, // 3  {0,0,1,1},
  {LOW, HIGH, LOW, LOW}, // 4  {0,1,0,0},
  {LOW, HIGH, LOW, HIGH}, // 5  {0,1,0,1),
  {LOW, HIGH, HIGH, LOW}, // 6  {0,1,1,0},
  {LOW, HIGH, HIGH, HIGH}, // 7  {0,1,1,1},
  {HIGH, LOW, LOW, LOW}, // 8  {1,0,0,0},
  {HIGH, LOW, LOW, HIGH} // 9  {1,0,0,1}
};

void setNixie(int nixieIndex, int number) {
  int nixieRegistersShift = 4 * nixieIndex;
  setRegisterPin(0 + nixieRegistersShift, numberNixieBinTab[number][0]);
  setRegisterPin(1 + nixieRegistersShift, numberNixieBinTab[number][1]);
  setRegisterPin(2 + nixieRegistersShift, numberNixieBinTab[number][2]);
  setRegisterPin(3 + nixieRegistersShift, numberNixieBinTab[number][3]);
  writeRegisters();
}


