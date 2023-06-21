#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include "DHTesp.h"

#define LED_BUILTIN 2
#define LDR_PIN 34
#define BUZZ_PIN 4
#define DHT_PIN 15
#define SERVO_PIN 18


WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dhtSensor;

char tempAr[6];
char humidityAr[6];
char IntensityAr[6];
bool  buzzerON = 0 ;


void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);



}

void loop() {
  

  if (!mqttClient.connected()){
    connectToBroker();
  }

  mqttClient.loop();

  
  updateIntensity();

  updateTempAndHumidity();
  //Serial.println(tempAr);
  //Serial.println(humidityAr);
  //Serial.println(IntensityAr);


  
  checkBuzzer(256,1);
  

  mqttClient.publish("TEMP",tempAr);
  delay(1000);
  mqttClient.publish("HUMIDITY",humidityAr);
  delay(1000);
  mqttClient.publish("INTENSITY",IntensityAr);
  //delay(1000);
  

}


void setupWiFi(){
    WiFi.begin("Wokwi-GUEST","");

    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");

    }
  Serial.println("");
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org",1883);
  mqttClient.setCallback(receiveCallback);

}

void connectToBroker(){
  while (!mqttClient.connected()){
    Serial.print("Attempting MQTT connection..");
    if (mqttClient.connect("ESP32-34534")){
      Serial.println("Connected");

      mqttClient.subscribe("Main-ON-OFF");

    }
    else{
      Serial.print("Failed");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}


void updateTempAndHumidity(){
  TempAndHumidity data =  dhtSensor.getTempAndHumidity();
  String(data.temperature,2).toCharArray(tempAr,6);
  String(data.humidity,2).toCharArray(humidityAr,6);

}

void receiveCallback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];

  for(int i = 0; i < length; i++ ){
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];

  }
  
  Serial.println();

  if ( strcmp(topic, "Main-ON-OFF") == 0 ){    //check the topic
    if (payloadCharAr[0]=='1'){
      digitalWrite(LED_BUILTIN,HIGH);
      buzzerON = 1;
      
    }
    else{
      digitalWrite(LED_BUILTIN,LOW);
      buzzerON = 0;

    }

  }

}


void updateIntensity(){
  delay(10); // this speeds up the simulation
  int LDRreading = analogRead(LDR_PIN);
  float LDRvalue = map(LDRreading, 4095, 0, 0,1000); 
  float mappedIntensity = LDRvalue/1000;
  String(mappedIntensity,4).toCharArray(IntensityAr,6);
  Serial.println(mappedIntensity);
 
}


  

void checkBuzzer(unsigned int freq , unsigned int del){
   if ( buzzerON == 1 ){    
      tone(BUZZ_PIN, freq);
      delay(del*1000);
      noTone(BUZZ_PIN);
      delay(del*1000);
    }
    else if ( buzzerON == 0 ){
      noTone(BUZZ_PIN);  
    }
  }




