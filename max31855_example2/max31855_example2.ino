// K型熱電対温度センサモジュールキット(SPI接続)MAX31855使用(5V版/3.3V版)サンプルスケッチ
// Example sketch for MAX31855 Type-K thermocouple sensor module (5V/3.3V) 
// Switch-Science 2013.5.14(Tue)

#include <SPI.h>

// Arduinoボードの電源電圧がキットの電源電圧と同じ場合、
// 以下のマクロを有効にしてモジュールを直挿しできます。
// If working voltage of your arduino board is same as the kit, 
// you can enable macro code next two lines
// to put the kit on your arduino directly.
/*
#define VCC  8
#define GND  9
*/
#define SLAVE 10

void setup() {
#ifdef GND
  pinMode(GND, OUTPUT);
  digitalWrite(GND, LOW); 
#endif
#ifdef VCC
  pinMode(VCC, OUTPUT);
  digitalWrite(VCC, HIGH);
#endif
  pinMode(SLAVE, OUTPUT);
  digitalWrite(SLAVE, HIGH);

  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);
}

void loop() {
  unsigned int thermocouple; // 14-Bit Thermocouple Temperature Data + 2-Bit
  unsigned int internal; // 12-Bit Internal Temperature Data + 4-Bit
  float disp; // display value 

  delay(500);
  digitalWrite(SLAVE, LOW);                             //  Enable the chip
  thermocouple = (unsigned int)SPI.transfer(0x00) << 8;   //  Read high byte thermocouple
  thermocouple |= (unsigned int)SPI.transfer(0x00);       //  Read low byte thermocouple 
  internal = (unsigned int)SPI.transfer(0x00) << 8;       //  Read high byte internal
  internal |= (unsigned int)SPI.transfer(0x00);           //  Read low byte internal 
  digitalWrite(SLAVE, HIGH);                            //  Disable the chip

  if((thermocouple & 0x0001) != 0) {
    Serial.print("ERROR: ");
    if ((internal & 0x0004) !=0) {
      Serial.print("Short to Vcc, ");
    }
    if ((internal & 0x0002) !=0) {
      Serial.print("Short to GND, ");
    }
    if ((internal & 0x0001) !=0) {
      Serial.print("Open Circuit, ");
    }    
    Serial.println();
  } else {
    if((thermocouple & 0x8000) == 0){ // 0℃以上   above 0 Degrees Celsius 
      disp = (thermocouple >> 2) * 0.25;
    } else {                          // 0℃未満   below zero
      disp = (0x3fff - (thermocouple >> 2) + 1)  * -0.25;
    }
    Serial.print(thermocouple, HEX);
    Serial.print(" : ");
    Serial.print(disp);

    Serial.print(" // ");
    
    if((internal & 0x8000) == 0){ // 0℃以上   above 0 Degrees Celsius
      disp = (internal >> 4) * 0.0625;
    } else {                          // 0℃未満   below zero
      disp = (((0xffff - internal) >> 4) + 1)  * -0.0625;
    }
    Serial.print(internal, HEX);
    Serial.print(" : ");
    Serial.print(disp);
    
    Serial.println();    
  }
}

