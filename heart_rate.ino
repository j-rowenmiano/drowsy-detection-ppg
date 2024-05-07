#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <LiquidCrystal_I2C.h>

MAX30105 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

int motor = 9; // motor pin
int led = 8; //led pin

float beatsPerMinute;
int beatAvg;


void setup()
{
  
  Serial.begin(9600);
  Serial.println("Initializing...");

  pinMode(motor,OUTPUT);
  pinMode(led, OUTPUT);
  
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST,0x57)) 
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  
  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Initializing");
  lcd.setCursor(3,1);
  lcd.print(".........");
  Serial.println("Place your index finger on the sensor with steady pressure.");
  Serial.println("");

  delay(1500);

  lcd.clear();
  lcd.print("Place your index");
  lcd.setCursor(0,1);
  lcd.print("finger on sensor");


  delay(2000);
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

if (irValue > 50000) //change for IR value threshold
{
      unsigned long currentTime = 0;
      unsigned long pastTime = 0;

      pastTime = currentTime;
      currentTime = millis();
      unsigned long timePassed = ((currentTime - pastTime)- 3700)/1000;
      unsigned long oneMinute = 60; //change value to any seconds threshold
      unsigned long interval = timePassed - oneMinute;
      
      beatAvg = 0;
      float beatAvg1;
      for (int l = 0; l < RATE_SIZE; l++)
      {

      beatsPerMinute = irValue / 1200.00;
      beatAvg1 += beatsPerMinute; 
      }

      beatAvg = beatAvg1 / RATE_SIZE;
      
      lcd.clear();
      Serial.print("IR = ");
      Serial.println(irValue);
      Serial.print("BPM = ");
      Serial.println(beatsPerMinute);
      Serial.print("Average BPM = ");
      Serial.println(beatAvg);
      
      Serial.print("Time Elapsed: ");
      Serial.print(timePassed);
      Serial.print(" seconds");
      Serial.println("");
      Serial.println("");
      
      lcd.print("IR=");
      lcd.print(irValue);
      lcd.setCursor(9,0);
      lcd.print("Time=");
      lcd.print(timePassed);
      lcd.setCursor(0,1);
      lcd.print("BPM=");
      lcd.print(beatsPerMinute);
      lcd.setCursor(9,1);
      lcd.print("AvgB=");
      lcd.print(beatAvg);

      delay(500);
      
  if (interval == oneMinute || timePassed == oneMinute)
  {

      Serial.print("One Minute passed!");
      Serial.println("");
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("One Minute");
      lcd.setCursor(5,1);
      lcd.print("Passed!");
      delay(1000);

      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Average BPM :");
      lcd.setCursor(7,1);
      lcd.print(beatAvg);
      delay(1000);
      
      if(beatAvg < 50) //change value to any threshold BPM
      {
        digitalWrite(motor, HIGH);
        digitalWrite(led, HIGH);
        Serial.print("Alarm ON");
        delay(500);
      }
      else
      {
        digitalWrite(motor, LOW);
        digitalWrite(led, LOW);
      } 
      delay(1000);
  }
}

  
else{
        irValue = 0;
        Serial.println("Please Insert Finger");
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Please insert");
        lcd.setCursor(5,1);
        lcd.print("Finger");
        delay(200);
}
   

}
