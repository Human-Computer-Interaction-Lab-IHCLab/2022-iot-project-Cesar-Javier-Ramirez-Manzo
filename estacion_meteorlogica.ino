
#include "UbidotsEsp32Mqtt.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SEALEVELPRESSURE_HPA (1013.25)
#define ALERT 04
#define LED_HUM  05
#define LED_TEMP 18
#define LED_PRE  19

Adafruit_BME280 bme; // I2C


const char *UBIDOTS_TOKEN = "BBFF-AKQxQ60ETH7BkqzOw9ZD2h2o6RGDcG";  
//const char *WIFI_SSID = "Galaxy S10+2228";      
//const char *WIFI_PASS = "xhcw8131";     
const char *WIFI_SSID = "LabArquitectura";      
const char *WIFI_PASS = "L4bArqui2022";     
const char *DEVICE_LABEL = "estacion-meteorologica";   

const int lim_temp=28;//menor a
const int lim_pres=900;//mayor a
const int lim_hum=90;//mayor a

const int PUBLISH_FREQUENCY = 5000; 

float temp;
float presion;
float altitud;
int humedad;
int tiempo;
unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  pinMode(LED_HUM,OUTPUT);
  pinMode(LED_TEMP,OUTPUT);
  pinMode(LED_PRE,OUTPUT);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  pinMode(ALERT,OUTPUT);
  digitalWrite(ALERT,LOW);
  timer = millis();
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  bool status;
  
  
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }


  Serial.println();

    
}


void loop() { 
  digitalWrite(LED_HUM,LOW);
  digitalWrite(LED_TEMP,LOW);
  digitalWrite(LED_PRE,LOW);
  delay(2500);
  tiempo=0;
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  temp =bme.readTemperature();
  Serial.println(" *C");
  if(temp<lim_temp){
    tiempo++;
    digitalWrite(LED_TEMP,HIGH);
  }

  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  presion = bme.readPressure() / 100.0F;
  Serial.println(" hPa");
  if(presion>lim_pres){
    tiempo++;
    digitalWrite(LED_PRE,HIGH);
  }
  

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  altitud= bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  humedad =bme.readHumidity();
  Serial.println(" %");
  if(humedad>lim_hum){
    tiempo++;
    digitalWrite(LED_HUM,HIGH);
  }
  Serial.println();
  Serial.print(tiempo);
  if(tiempo>=2){
    digitalWrite(ALERT,HIGH);
  }else{
    digitalWrite(ALERT,LOW);
  }
   ubidots.add("Temperatura",temp); 
   ubidots.add("Humedad",humedad);
   ubidots.add("Presion",presion);
   ubidots.add("Altitud",altitud);
   ubidots.add("Tiempo",tiempo);
   ubidots.publish(DEVICE_LABEL);
  
  ubidots.loop();
  delay(2500);
}
