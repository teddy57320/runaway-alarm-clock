#include <SPI.h>
#include <RH_NRF24.h>
#include "pitches.h"

RH_NRF24 nrf24;   //transceiver (server)

//PINS 10 to 13 and 8 used for NRF24

int leftPin = 5;
int rightPin = 6;
int echoPin = 16;
int trigPin = 17;
int speakerPin = 9;
int stopPin = 14;

int maxRange = 200; // Maximum range needed
int minRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

int melody[] = {
  NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4
};

void setup() 
{
  Serial.begin(9600);
  while (!Serial);
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(4))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    

  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(stopPin, INPUT);
}

void loop()
{

  Serial.println("standby...");
  if (nrf24.available())
  {
    // message for us to receive 
    
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.print("Received signal: ");
      Serial.println((char*)buf);         //Will display "Alarm has gone off!"
      
      while(analogRead(stopPin)){         //while stop button is not pushed

         digitalWrite(trigPin, LOW);      //ultrasound sensor
         delayMicroseconds(2); 
         digitalWrite(trigPin, HIGH);
         delayMicroseconds(10); 
         digitalWrite(trigPin, LOW);
         duration = pulseIn(echoPin, HIGH);
         //Calculate the distance (in cm) based on the speed of sound.
         distance = duration/58.2;

         if (distance <= 140){             
          //adjust wheel speeds based on whether or not an object is within proximity
            analogWrite(rightPin, 200);
            analogWrite(leftPin, 0);
         }
         else{
            analogWrite(rightPin, 255);
            analogWrite(leftPin, 255);
         }
         for (int thisNote = 0; thisNote < 1; thisNote++) {                
         //play alarm music
            int noteDuration = 800 / noteDurations[thisNote];
            tone(speakerPin, melody[thisNote], noteDuration);
            int pauseBetweenNotes = noteDuration * 1.20;
            delay(pauseBetweenNotes);
            // stop tone
            noTone(speakerPin);
         }
       }
       analogWrite(rightPin, 0);
       analogWrite(leftPin, 0);
       uint8_t data[] = "Alarm turned off!";       //send response to client 
       nrf24.send(data, sizeof(data));
       nrf24.waitPacketSent();
       Serial.println("Alarm has been turned off.");
    }
    else
      Serial.println("receiver failed");
  }
  delay(1000); 
  }
  

