#include <PubSubClient.h>
#include <WiFi.h>
#include "DHTesp.h"

const int DHT_PIN = 15;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dhtSensor;

char tempAr[6];
char humidityAr[6];

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);


}

void loop() {
  if (!mqttClient.connected()){
    connectToBroker();
  }

  mqttClient.loop();

  updateTempAndHumidity();
  Serial.println(tempAr);
  Serial.println(humidityAr);
  mqttClient.publish("TEMP",tempAr);
  delay(1000);
  mqttClient.publish("HUMIDITY",humidityAr);
  delay(1000);

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
  
  Serial.pritln();
}






