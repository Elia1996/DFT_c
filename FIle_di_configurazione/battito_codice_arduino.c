+
/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;


// COSTANTI PER LA MEDIA
// battiti in cui conta il tempo
const int numberOfBeat=5; // è il numero di BPM da mediare
const int NOB=2; // è il numero di battiti da contare prima di calcolare i BPM

// TEMPI
// tempo dell'ultimo battito
long lastBeat = 0;
// per tenere traccia del tempo da mandare via seriale
long lastMillis =0;

// CALCOLO DELLA MEDIA
float vecBeat[NOB]={0}; //Array of heart rates
float beatSum=0; 
float beatAvg=0;
float beatsPerMinute;

// CONTATORI
int beatCount=0;
int vecBeatCount=0;


int count(int cnt){
  return (cnt+NOB)%NOB;
}

int16_t acIr;

void setup()
{
        Serial.begin(115200);
        //Serial.println("Initializing...");
        //Serial.println(I2C_SPEED_FAST);
        //Serial.println("ciao");
        // Initialize sensor
        while (!particleSensor.begin(Wire,I2C_SPEED_FAST)){//I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed{
                Serial.println("MAX30105 was not found. Please check wiring/power. ");
          //      Serial.println("ciao");
        }
       //Serial.println("ciao");
        //Serial.println("Place your index finger on the sensor with steady pressure.");
        //              bool print_peak, bool print_AC_current,bool detect2_on
        heartRateSetup(false,false,false);
        //setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 3200, int pulseWidth = 217, int adcRange = 4095
        particleSensor.setup(0x1F,4,2,200,217,4095); //Configure sensor with default setting
        particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
        particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}


void loop()
{
        long irValue = particleSensor.getIR();
        
        if (checkForBeat(irValue,&acIr) == true && irValue >= 50000) {
                beatCount++;
                if (beatCount>=numberOfBeat){
                        // TEMPO
                        long delta = millis() - lastBeat;
                        lastBeat = millis();
                        // CALCOLO DEL BPM
                        beatsPerMinute = (60 / (delta / 1000.0))*numberOfBeat;
                        // salvo il battito nel vettore
                        vecBeat[vecBeatCount] = beatsPerMinute;
                        // somma dei battiti
                        beatSum = beatSum + vecBeat[vecBeatCount] - vecBeat[count(vecBeatCount+1)];
                        // media battiti
                        beatAvg = beatSum/(NOB-1);
                        
                        // CONTATORI
                        vecBeatCount = count(vecBeatCount+1);
                        beatCount = 0;
                        }
        }
        if (irValue < 50000)
              Serial.print(" No finger?");
        Serial.println(acIr);
        //Serial.print(" ");      
        //Serial.println(millis()-lastMillis);
        //lastMillis=millis();
}
