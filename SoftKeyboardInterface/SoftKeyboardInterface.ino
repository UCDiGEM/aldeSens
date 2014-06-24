/* aldeSens - UC Davis iGEM 2014
/
*/
#include "ADC.h"


const int light1 = 11;
const int light2 = 12;
const int ledPin = 13;
const int readPin = A9;
elapsedMicros usec = 0;



ADC *adc = new ADC(); // adc object

RingBuffer *buffer = new RingBuffer;

void setup() {

  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin, INPUT);
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);

  analogWriteResolution(12);

  adc->setAveraging(16); // set number of averages
  adc->setResolution(16); // set bits of resolution
  adc->setAveraging(16, ADC_1); // set number of averages
  adc->setResolution(16, ADC_1); // set bits of resolution

  while (usec < 5000); // wait
  usec = usec - 5000;
}



String inStruct;                                                                                                                                                                                  ;
double value = 0;
int count = 1;
int params;

void loop() {

  mainController();

}

void mainController() {
   if (Serial.available()) {
    // Interpret commands from computer
      inStruct = Serial.readStringUntil(',');
    }
// Instruction List:
//    lightOn: turn pin 11 LED on
//    lightOFF: turn pin 11 LED off
//    cycVolt#a#b#c#d: Cyclic Voltametry
//                          a# (2 digits):    Sampling Time (secs)
//                          b# (2 digits):    Starting Voltage (volts)
//                          c# (2 digits):    Ending Voltage (volts)
//                          d# (any digits):  Scan Rate (volts/second)
//
//
//
    if(inStruct=="lightOn") { 
      digitalWrite(light2, HIGH);
    }
    
    else if(inStruct=="lightOff") { 
      digitalWrite(light2, LOW);
    } 
    
    else if(inStruct.startsWith("cycVolt")) { 
           
      int sampTime    = inStruct.substring(7,9).toInt();
      int startVolt   = inStruct.substring(9,11).toInt();
      int endVolt     = inStruct.substring(11,13).toInt();
      int scanRate   = inStruct.substring(13).toInt();
      cycVolt(sampTime, startVolt, endVolt, scanRate);
    } 
}

// sampTime in seconds (Sampling time)
/// startVolt in volts
// endVolt in volts
// sweepRate

void cycVolt(int sampTime, int startVolt, int endVolt, int scanRate) {
      int length = sampTime*2000; // With delay of 5 µs, 2000 samples per second
      for (int i = 0; i < length; i++) {
        value = adc->analogRead(readPin);

        buffer->write(value);

        Serial.println(buffer->read()*3.3/adc->getMaxValue(ADC_0),5);
        while (usec < 5); // wait
        usec = usec - 5;
      }
      inStruct = "";
}



