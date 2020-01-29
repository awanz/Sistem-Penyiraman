// BLYNK
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>;
#include <BlynkSimpleEsp8266.h>;
char auth[] = "9NIFEbn7f41z628boitZf5Ye_tUAserV";
char ssid[] = "XXXXXXXXX"; //nama wifi
char pass[] = "XXXXXXXXX"; //password wifi

//DHT
#include <DHT.h>
#define DHTPIN 0          // D3
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

//LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//ADS
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0x48);

//Status Auto
int auto1 = 0;
int auto2 = 0;

// Relay
int pinRelay1 = 14; // D5
int pinRelay2 = 10; // SD3

//Status Relay
int statusRelay1 = 0;
int statusRelay2 = 0;

// Sensor Tanah
const int pinKelembabanTanah = 0; // A0
int16_t kelembabanTanah = 0;
const int pinKelembabanTanah2 = 1; 
int16_t kelembabanTanah2 = 0;

int batasKelembabanTanah = 10;

void sendSensor()
{
  //  suhu dan kelembaban
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Kelembaban Tanah
  kelembabanTanah = ads.readADC_SingleEnded(0);
  kelembabanTanah = map(kelembabanTanah,0,65535,0,100);

  // Kelembaban Tanah 2
  kelembabanTanah2 = ads.readADC_SingleEnded(1);
  kelembabanTanah2 = map(kelembabanTanah2,0,65535,0,100);

  //  Serial Print
  Serial.println();
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humadity: ");
  Serial.println(h);
  Serial.print("Moisture 1: ");
  Serial.println(kelembabanTanah);
  Serial.print("Moisture 2: ");
  Serial.println(kelembabanTanah2);
  Serial.print("Auto 1: ");
  Serial.println(auto1);
  Serial.print("Auto 2: ");
  Serial.println(auto2);
  Serial.print("Status Relay 1: ");
  Serial.println(statusRelay1);
  Serial.print("Status Relay 2: ");
  Serial.println(statusRelay2);
  Serial.println();

  //  LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.setCursor(2, 0);
  lcd.print(t);
  lcd.setCursor(5, 0);
  lcd.print("H:");
  lcd.setCursor(7, 0);
  lcd.print(h);
  lcd.setCursor(12, 0);
  lcd.print("P1:");
  lcd.setCursor(15, 0);
  lcd.print(statusRelay1);
  
  lcd.setCursor(0, 1);
  lcd.print("M1:");
  lcd.setCursor(3, 1);
  lcd.print(kelembabanTanah);
  lcd.setCursor(6, 1);
  lcd.print("M2:");
  lcd.setCursor(9, 1);
  lcd.print(kelembabanTanah2);
  lcd.setCursor(12, 1);
  lcd.print("P2:");
  lcd.setCursor(15, 1);
  lcd.print(statusRelay2);
  
  // virtual
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, kelembabanTanah);
  Blynk.virtualWrite(V4, kelembabanTanah2);
}

//status auto 1
BLYNK_WRITE(V20) 
{
  auto1 = param.asInt();
}

//status auto 2
BLYNK_WRITE(V21) 
{
  auto2 = param.asInt();
}

//keadaan pompa 1
BLYNK_WRITE(V10) 
{
  statusRelay1 = param.asInt();
  if(statusRelay1){
    digitalWrite(pinRelay1, HIGH);
  }else{
    digitalWrite(pinRelay1, LOW);
  }
}

//keadaan pompa 2
BLYNK_WRITE(V11) 
{
  statusRelay2 = param.asInt();
  if(statusRelay2){
    digitalWrite(pinRelay2, HIGH);
  }else{
    digitalWrite(pinRelay2, LOW);
  }
}

void setup(){
  //  Serial
  Serial.begin(9600);
  Serial.println("Starting...");

  //  LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  //  ADS
  ads.begin();

  //  DHT
  dht.begin();
  
  //  Blynk
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendSensor);
  
  pinMode(pinKelembabanTanah, INPUT);
  pinMode(pinKelembabanTanah2, INPUT);
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, LOW);
  
  delay(3000);
  lcd.clear();
}
 
void loop(){
  Blynk.run();
  timer.run();

  if(auto1){
    if(kelembabanTanah < batasKelembabanTanah){
      digitalWrite(pinRelay1, HIGH);
      delay(1000);
      digitalWrite(pinRelay1, LOW);
    }
  }

  if(auto2){
    if(kelembabanTanah2 < batasKelembabanTanah){
      digitalWrite(pinRelay2, HIGH);
      delay(1000);
      digitalWrite(pinRelay2, LOW);
    }
  }
}
