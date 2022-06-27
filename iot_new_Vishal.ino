//-----------------Group 4 CP----------------//
#include "ThingSpeak.h"
#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//----------- Enter you Wi-Fi Details---------//
char ssid[] = "realme narzo 20A"; //SSID
char pass[] = "mankotia"; // Password
//-------------------------------------------//

// ---------- garbage bin details ------------//
const int total_height = 30; // garbage bin height in CM
const int hold_height = 25;// garbage holding capacity (height) in CM
//-------------------------------------------//

//----- Minutes -----//
int Minute = 1; // Data update in min.
//------------------//

//----------- Channel Details -------------//
unsigned long Channel_ID = 1717377; // Channel ID
const char * WriteAPIKey = "KNB3EKK3LAFZNXLS"; // Your write API Key
// ----------------------------------------//

const int trigger = 14;
const int echo = 12;
long Time;
int x;
int i;
int distanceCM;
int resultCM;
int bin_lvl = 0;
int snsr_to_max = 0;
const int Field_number = 1; 
WiFiClient  client;

void setup()
{
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  Wire.begin(2,0);
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("IoT Garbage lvl");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Sys.");
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  snsr_to_max = total_height - hold_height;
  delay(250);

  Runable();
}
void loop()
{

  if(bin_lvl >= 95){
    lcd.clear();
    digitalWrite(D8, LOW); 
    digitalWrite(D7, HIGH); // turn the LED off by making the voltage LOW
    lcd.print("Bin is Full");
    Serial.print("Garbage Bin is Full");
    delay(15000);
    lcd.clear();
    Runable();
  delay(1000);
  }
  else{
    digitalWrite(D8, HIGH);
    digitalWrite(D7, LOW);// turn the LED on (HIGH is the voltage level)
    Runable();
    
  
    }
  
}
 void Runable(){
  internet();
  measure();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Garbage Level:");
  lcd.setCursor(5, 1);
  lcd.print(bin_lvl);
  lcd.print('%');
  Serial.print("Garbage Level:");
  Serial.print(bin_lvl);
  Serial.println("%");
  upload();
  for (i = 0; i < Minute; i++)
  {
    Serial.println("-------------------------");
    Serial.println("System Standby....");
    Serial.print(i);
    Serial.println(" Minutes elapsed.");
    delay(200);
    delay(200);
    delay(200);
  }
 }

void upload()
{
  internet();
  x = ThingSpeak.writeField(Channel_ID, Field_number, bin_lvl, WriteAPIKey);

  if (x == 200)Serial.println("Data Updated.");
  if (x != 200)
  {
    Serial.println("Data updating....");
    delay(15000);
    upload();
  }
}

void measure()
{
  delay(100);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  Time = pulseIn(echo, HIGH);
  distanceCM = Time * 0.034;
  resultCM = distanceCM / 2;

  bin_lvl = map(resultCM, snsr_to_max, total_height, 100, 0);
  if (bin_lvl > 100) bin_lvl = 100;
  if (bin_lvl < 0) bin_lvl = 0;
}

void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}
