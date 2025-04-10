#include <Wire.h>
//merging eda and heart rate
#include "Arduino_BHY2.h"
#include "AD5593R.h"

#include "mbed.h"
#include "MAX30105.h"
#include "heartRate.h"

// #include "mbed.h"

MAX30105* particleSensor;




// int errorCount = 0;  // Counts consecutive errors before reset
unsigned long startTime = 0;  
long int R1=920000;

// float vcc=2.496;

AD5593R* adac;


long lastBeat = 0;



void setup() {
  Serial.begin(9600);
  while (!Serial);


  BHY2.begin();
  Wire.begin();
  //  temp.begin();
  particleSensor= new MAX30105();
  if (!particleSensor->begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check connections.");
    while (1);
  }
  particleSensor->setup();
  particleSensor->setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running

  adac = new AD5593R();          // Now create the object safely
  adac->enable_internal_Vref();
  adac->set_DAC_max_1x_Vref();
  adac->set_ADC_max_1x_Vref();
  adac->configure_DAC(0);
  adac->write_DAC(0, 2);
  // adac->configure_DAC(1);
  // adac->write_DAC(1, 1.5);

// Configure IO2 as ADC
  adac->configure_ADC(2);
  adac->configure_ADC(3);
  adac->configure_ADC(4);

  startTime = millis(); 

}


void loop() {
  BHY2.update();// Serial.println("\nScanning I2C devices...");
  unsigned long elapsedTime = (millis() - startTime) / 1000; // in seconds

  float vO = adac->read_ADC(2);
  float vb = adac->read_ADC(3);
  float vcc = adac->read_ADC(4);

  Serial.print(vcc,5);
  Serial.print(',');
  Serial.print(vO,5);
  Serial.print(',');
  Serial.print(vb,5);
  Serial.print(',');
  float Rmeas=((vcc-vb)/(vb-vO))*R1;
  float EDA=1000000/Rmeas;

  Serial.print(EDA,5);
  Serial.print(',');
  long irValue = particleSensor->getIR();
  Serial.println(irValue);
  
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    float beatsPerMinute = 60 / (delta / 1000.0);
    // Serial.print("BPM: ");
    // Serial.println(beatsPerMinute);
  }

  delay(50);



}








