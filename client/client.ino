#include <SPI.h>
#include <RH_NRF24.h>
#include <Wire.h>
#include "RTClib.h"

RH_NRF24 nrf24;   //transceiver 

RTC_PCF8523 rtc;  //real time clock

//PINS 10 to 13 and 8 used for NRF24
unsigned char alarmSet = 0;  //alarm is initially off
String message;              //reads user input from Serial
char alarmTime[6];           //time of the alarm
int mins, secs;              //minutes and seconds of alar 

void setup() 
{
  Serial.begin(9600);
  if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
  }
  while (!Serial);
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(4))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
}

void loop()
{
  if(Serial.available()){   //when user inputs message to Serial
      message = Serial.readString();   
      if (message == "On"){   //turn on alarm
        Serial.println("Please enter the time of the alarm in MM:SS");
        while (!Serial.available())
            delay(10);
        String str = Serial.readString();
        Serial.print("Alarm has been set for ");
        Serial.println(str);
        str.toCharArray(alarmTime, 6);
        mins = alarmTime[0]*10 + alarmTime[1] - 528;  //alarm minutes
        secs = alarmTime[3]*10 + alarmTime[4] - 528;  //alarm seconds
        alarmSet = 1;
      }
      else if (message == "Off"){
        Serial.println("Alarm has been turned off");
        alarmSet = 0;
      }
      else
        Serial.println("Please enter a valid command");
  }
  if (alarmSet)
      Serial.println("Alarm is on. Enter 'Off' to turn it off.");
  else
      Serial.println("Alarm is off. Enter 'On' to turn alarm on");
  DateTime currTime = rtc.now();
  if (alarmSet){  
     if (currTime.minute() == mins && currTime.second() == secs){        
        uint8_t data[] = "Alarm has gone off!";       //send signal for alarm to sound
        nrf24.send(data, sizeof(data));       
        nrf24.waitPacketSent();
        delay(10);
        Serial.println("Alarm has gone off!");
     }
  }
          
  if(nrf24.available()){      // waiting for user to turn off alarm
      uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (nrf24.recv(buf, &len)){
         Serial.print("Response: ");
         Serial.println((char*)buf);   //Will display "Alarm turned off!"
         alarmSet = 0;      
      }
      else
         Serial.println("Receiver failed");
   
  }
  Serial.print(currTime.hour(), DEC);   //print real time 
  Serial.print(':');
  if (currTime.minute()<10)
    Serial.print("0");
  Serial.print(currTime.minute(), DEC);
  Serial.print(':');
  if (currTime.second()<10)
    Serial.print("0");
  Serial.print(currTime.second(), DEC);
  Serial.println();
  delay(900);
  
}

