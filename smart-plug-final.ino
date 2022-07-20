#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;
char auth[] = "n4nKwwStIOgQqEOcqQUfbAUkpY5Pvapj";

char ssid[] = "#######";
char pass[] = "#########";

// Set your LED and physical button pins here
const int ledPin = D0;
const int PUSH_BUTTON = D1;
const int led = D5;
#define Vpin V0
int ledState = LOW;
int pushButtonState = HIGH;
const int sensorIn = A0;
int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
BLYNK_CONNECTED()
{
  Blynk.syncVirtual(Vpin);
}
void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON) == LOW) {
    // pushButtonState is used to avoid sequential toggles
    if (pushButtonState != LOW) {

      // Toggle Relay state
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      digitalWrite(led, !ledState);

      // Update Button Widget
      Blynk.virtualWrite(Vpin, ledState);
    }
    pushButtonState = LOW;
  } else {
    pushButtonState = HIGH;
  }
  Blynk.virtualWrite(Vpin,ledState);
}
BLYNK_WRITE(Vpin) {
  ledState = param.asInt();
  digitalWrite(ledPin, ledState);
  digitalWrite(led, !ledState);
}
void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth,ssid,pass);
  pinMode(ledPin, OUTPUT);  
  pinMode(led, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  digitalWrite(Vpin, ledState);
  digitalWrite(led, ledState);
  timer.setInterval(500L, checkPhysicalButton);
}

void loop()
{
  Blynk.run();
  timer.run();
  Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; // sq root
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 float Wattage = (220*AmpsRMS)-18; //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS ");
 Serial.print(Wattage); 
 Serial.println(" Watt ");
 Blynk.virtualWrite(V1, Wattage);
 Blynk.virtualWrite(V2, AmpsRMS);
 
}
float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();

   while((millis()-start_time) < 100) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
/*       Serial.print(readValue);
       Serial.println(" readValue ");
       Serial.print(maxValue);
       Serial.println(" maxValue ");
       Serial.print(minValue);
       Serial.println(" minValue ");
       delay(1000); */
    }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5)/1024.0;
      
   return result;
 }
