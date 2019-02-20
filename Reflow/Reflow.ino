#include <SPI.h>

#define SLAVE 10

#define TOP 5 //Heater Top
#define BOTTOM 6 //Heater Bottom

void setup()
{
  pinMode(SLAVE, OUTPUT);
  digitalWrite(SLAVE, HIGH);

  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);

  initialheat(170, 30000);  //実動時:80sec(80000usec)
  Serial.println("Finish Heating!!");
  mainheat(240,30000);
  Serial.println("Finish Heating!!");
}

void loop()
{
  gettmp();
  delay(500);
}

float gettmp(void)
{
  unsigned int thermocouple; // 14-Bit Thermocouple Temperature Data + 2-Bit
  unsigned int internal; // 12-Bit Internal Temperature Data + 4-Bit
  float Furnace;
  float Internal; // display value

  delay(500);
  digitalWrite(SLAVE, LOW);                             //  Enable the chip
  thermocouple = (unsigned int)SPI.transfer(0x00) << 8;   //  Read high byte thermocouple
  thermocouple |= (unsigned int)SPI.transfer(0x00);       //  Read low byte thermocouple
  internal = (unsigned int)SPI.transfer(0x00) << 8;       //  Read high byte internal
  internal |= (unsigned int)SPI.transfer(0x00);           //  Read low byte internal
  digitalWrite(SLAVE, HIGH);                            //  Disable the chip

  if((thermocouple & 0x0001) != 0)
  {
    Serial.print("ERROR: ");
    if ((internal & 0x0004) !=0)
      Serial.print("Short to Vcc, ");
    if ((internal & 0x0002) !=0)
      Serial.print("Short to GND, ");
    if ((internal & 0x0001) !=0)
      Serial.print("Open Circuit, ");
    Serial.println();
  }
  else
  {
    if((thermocouple & 0x8000) == 0)
    { // 0℃以上   above 0 Degrees Celsius
      Furnace = (thermocouple >> 2) * 0.25;
    }
    else
    { // 0℃未満   below zero
      Furnace = (0x3fff - (thermocouple >> 2) + 1)  * -0.25;
    }
    Serial.print(thermocouple, HEX);
    Serial.print(" : ");
    Serial.print(Furnace);

    Serial.print(" // ");

    if((internal & 0x8000) == 0)
    { // 0℃以上   above 0 Degrees Celsius
      Internal = (internal >> 4) * 0.0625;
    }
    else
    { // 0℃未満   below zero
      Internal = (((0xffff - internal) >> 4) + 1)  * -0.0625;
    }
    Serial.print(internal, HEX);
    Serial.print(" : ");
    Serial.print(Internal);

    Serial.println();

    return(Furnace);
  }
}

void initialheat(float tmpTarget, int timeRetention)
{
  float tmpFurnace;
  tmpFurnace = gettmp();
  boolean heating = true; //  加熱中:true，冷却中:false

  while(1)  //加熱
  {
    analogWrite(TOP,255);
    analogWrite(BOTTOM,255);   
    tmpFurnace = gettmp();
    if(tmpTarget <= tmpFurnace)
    {
      analogWrite(TOP,0);
      analogWrite(BOTTOM,0);
      heating =false;
      break;
    }
  }

  unsigned long retstart;
  retstart = millis();

  while(millis()-retstart < timeRetention)//保温
  {
    tmpFurnace = gettmp();
    if(tmpFurnace <= tmpTarget)
    {
      analogWrite(TOP,255);
      analogWrite(BOTTOM,255);
      delay(10);
    }
    else
    {
      analogWrite(TOP,0);
      analogWrite(BOTTOM,0);
      delay(10);
    }
  }
  analogWrite(TOP,0);
  analogWrite(BOTTOM,0);
}

void mainheat(float tmpTarget, int timeRetention)
{
  float tmpFurnace;
  tmpFurnace = gettmp();
  boolean heating = true; //  加熱中:true，冷却中:false

  while(1)  //加熱
  {
    analogWrite(TOP,255);
    analogWrite(BOTTOM,255);   
    tmpFurnace = gettmp();
    if(tmpTarget-2 <= tmpFurnace)
    {
      analogWrite(TOP,0);
      analogWrite(BOTTOM,0);
      heating =false;
      break;
    }
  }

  unsigned long retstart;
  retstart = millis();

  while(millis()-retstart < timeRetention)//保温
  {
    tmpFurnace = gettmp();
    if(tmpFurnace <= tmpTarget-2)
    {
      analogWrite(TOP,255);
      analogWrite(BOTTOM,255);
      delay(10);
    }
    else if(tmpFurnace <= tmpTarget)
    {
      analogWrite(TOP,150);
      analogWrite(BOTTOM,100);
      delay(10);
    }
    else
    {
      analogWrite(TOP, 100);
      analogWrite(BOTTOM, 70);
      delay(10);
    }
    
  }
  analogWrite(TOP,0);
  analogWrite(BOTTOM,0);
}