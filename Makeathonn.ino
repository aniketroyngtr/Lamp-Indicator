//for wifi
#include <WiFi.h>
// Replace with your network credentials
const char* ssid = "OnePlus Nord";
const char* password = "ruchiarunim";
//for thinkspeak
#include "ThingSpeak.h"
unsigned long channelId = 2084114; // replace YOUR_CHANNEL_ID with your actual channel ID
const char* apiKey = "TDAQGE3R3QCWNMYA"; // replace YOUR_API_KEY with your actual API key
// Set up Ultrasonic sensor pins
const int trig1 = 5;                                                                            
const int echo1 = 18;
const int trig2 = 19;
const int echo2 = 21;

// Set up LED pin
#define bulb 14

// Set up LDR pins
#define ldr1 32
#define ldr2 35

// Set up LDR thresholds
#define Threshold_ldr1 1800
#define Threshold_ldr2 1800

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
//#define CM_TO_INCH 0.393701

long duration1;
long duration2;
float distance1;
float distance2;
float dist=100;      // threshold for the ultrasonic sensor
//float distanceInch;
int intensity= 0;
int fault=0;
int number_of_detections=0;
WiFiClient  client;

void setup() {
  // Set LED pin mode
  Serial.begin(115200); // Starts the serial communication
  pinMode(bulb, OUTPUT);
//  code for wifi connection
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  ThingSpeak.begin(client); // initialize the ThingSpeak library with your Ethernet client
  Serial.println("Connected to WiFi");
  
  pinMode(trig1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo1, INPUT); // Sets the echoPin as an Input
  pinMode(trig2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo2, INPUT); // Sets the echoPin as an Input
}
  // setup for think speak
  
  // Set up Serial communication for debugging
 


void loop() {
  
// check if sun is down or not
int ldr1Value = analogRead(ldr1);
Serial.print("ldr1Value: ");
Serial.print(ldr1Value);
if (ldr1Value < Threshold_ldr1) 
{ 
  // turn on the led for some value
  intensity=64;
  analogWrite(bulb, intensity);
  // Read Ultrasonic sensor distances
  digitalWrite(trig1, LOW);
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds 
  digitalWrite(trig1, HIGH);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  digitalWrite(trig2, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echo1, HIGH);
  duration2= pulseIn(echo2, HIGH);
  
  // Calculate the distance
  distance1 = duration1 * SOUND_SPEED/2;
  distance2 = duration2 * SOUND_SPEED/2;
  
  // Check if there is a person detected
  if (distance1 < dist || distance2 < dist) {
    // Increase LED intensity if person detected
    number_of_detections++;
    intensity+= 5;
    if (intensity> 255) {
      intensity= 255;
      
    }
  } else {
    // Turn off LED if no person detected
     intensity-= 5;
    if (intensity< 0) 
    {
      intensity= 0;
    }
  }
    analogWrite(bulb, intensity);

  // Read LDR sensor values
  
  int ldr2Value = analogRead(ldr2);
  Serial.print("ldr2Value: ");
  Serial.print(ldr2Value);
    if (ldr2Value < Threshold_ldr2) {
      fault=1;
    Serial.println("Bulb Faulty");
  } else {
    fault=0;
    //Serial.println("Bulb Working");
  }
  
ThingSpeak.setField(1, ldr1Value); // send the value of LDR1 to field 1
ThingSpeak.setField(2, number_of_detections); // send the number of detections to field 2
ThingSpeak.setField(3, fault); // send the status of the bulb to field 3
ThingSpeak.setField(4, intensity); // send the LED intensity to field 4
ThingSpeak.writeFields(channelId, apiKey); // send the data to ThingSpeak 
  

}
else {
  intensity= 0;
   fault=0;
  ThingSpeak.setField(3, fault); // send the status of the bulb to field 3
  ThingSpeak.setField(4, intensity); // send the LED intensity to field 4
  ThingSpeak.writeFields(channelId, apiKey); // send the data to ThingSpeak 
 
    analogWrite(bulb,intensity);
  }
  // Wait for 500ms before looping again
  delay(500);
}
