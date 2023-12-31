#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <RTClib.h>
#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>

#define LED_BUILTIN 2
#define LDR_PIN 34
#define BUZZ_PIN 4
#define DHT_PIN 15
#define SERVO_PIN 18

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dhtSensor;
Servo servo;
LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);


char tempAr[6];
char humidityAr[6];
char IntensityAr[6];
bool buzzerON = 0;
bool mainON = 0;
bool isAlarmTriggered = 0;
bool isBuzContinous = 0;
int AlarmDelay = 1;           // default
int Frequency = 300;         // default
int minAngle = 30;           // default
float controlFactor = 0.75;  // default
float Intensity;
int servoAngle = 0;



String dateTime;
String date;
String Alarm1;
String Alarm2;
String Alarm3;




void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  servo.attach(SERVO_PIN, 500, 2400);

  LCD.init();
  LCD.backlight();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  if (!mqttClient.connected()) {
    connectToBroker();
  }

  mqttClient.loop();

  updateIntensity();

  updateTempAndHumidity();




  checkBuzzer(Frequency, AlarmDelay);


  adjustServo();



  mqttClient.publish("TEMP", tempAr);
  delay(100);
  mqttClient.publish("HUMI", humidityAr);
  delay(100);
  mqttClient.publish("INTENSITY", IntensityAr);
  
}

void setupWiFi() {
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
}

void setupMqtt() {
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);
}

void connectToBroker() {  // subscribe to topics
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection..");
    if (mqttClient.connect("ESP32-34534")) {
      Serial.println("Connected");

      mqttClient.subscribe("Main-ON-OFF");
      delay(10);
      mqttClient.subscribe("AlarmDelay");
      delay(10);
      mqttClient.subscribe("freq");
      delay(10);
      mqttClient.subscribe("dropDown");
      delay(10);
      mqttClient.subscribe("min-Angle");
      delay(10);
      mqttClient.subscribe("CF");
      delay(10);
      mqttClient.subscribe("alramTrigger");
      delay(10);
      mqttClient.subscribe("dateTime");

    } else {
      Serial.print("Failed");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void updateTempAndHumidity() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String(data.temperature, 2).toCharArray(tempAr, 6);
  String(data.humidity, 2).toCharArray(humidityAr, 6);
}

void receiveCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }

  Serial.println();


  if (strcmp(topic, "Main-ON-OFF") == 0) {  // check the topic
    if (payloadCharAr[0] == '1') {
      digitalWrite(LED_BUILTIN, HIGH);
      buzzerON = 1;
      mainON = 1;
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      buzzerON = 0;
      mainON = 0;
    }
  }



  else if (strcmp(topic, "alramTrigger") == 0) {  // check the topic
    if (length == 1)                              // if an alram is triggered the payload length = 1
    {
      buzzerON = 1;
      isAlarmTriggered = 1;
    } else if (mainON == 0)  // buzzer is switched off only if the main switch is off
    {

      buzzerON = 0;
      isAlarmTriggered = 0;
    }
    else {
      isAlarmTriggered = 0;
    }


  }

  else if (strcmp(topic, "dateTime") == 0) {  // Getting the relavant info from the payload
    date = String(payloadCharAr);
    Serial.println(date);
    dateTime = date.substring(0, 16);
    Alarm1 = date.substring(17, 22);
    Alarm2 = date.substring(23, 28);
    Alarm3 = date.substring(29, 34);

    updateDisplay();

  }



  else if (strcmp(topic, "dropDown") == 0) {  // check the topic
    if (payloadCharAr[0] == '1') {
      isBuzContinous = 0;  //repeated on-off

    } else {
      isBuzContinous = 1;  //continuous -> no delay
      AlarmDelay = 0;
    }
  }

  else if (strcmp(topic, "AlarmDelay") == 0) {  // check the topic
    AlarmDelay = atoi((char *)payload);
  }

  else if (strcmp(topic, "freq") == 0) {  // check the topic
    Frequency = atoi((char *)payload);
  }

  else if (strcmp(topic, "min-Angle") == 0) {  // check the topic
    minAngle = atoi((char *)payload);
    if (minAngle > 100) {
      minAngle = minAngle / 10;
    }
  }

  else if (strcmp(topic, "CF") == 0) {  // check the topic
    controlFactor = atof((char *)payload);
  }
}

void updateIntensity() {
  delay(10);  // this speeds up the simulation
  int LDRreading = analogRead(LDR_PIN);
  float LDRvalue = map(LDRreading, 4095, 0, 0, 1000);
  Intensity = LDRvalue / 1000;
  String(Intensity, 4).toCharArray(IntensityAr, 6);
  
}

void checkBuzzer(unsigned int freq, unsigned int del) {
  if (buzzerON == 1) {

    if (isBuzContinous == 1)

    {
      while (1) {
        tone(BUZZ_PIN, freq);
        mqttClient.loop();     
        if (buzzerON == 0 || isAlarmTriggered == 0 ){
          break ;
        }
      }


    }

    else if (isBuzContinous == 0)  // repeated on off
      tone(BUZZ_PIN, freq);
    delay(1000);
    noTone(BUZZ_PIN);
    delay(del * 1000);
  }

  else if (buzzerON == 0) {
    noTone(BUZZ_PIN);
  }
}

void adjustServo() {
  servoAngle = minAngle + (180 - minAngle) * Intensity * controlFactor;  // calculating servo angle
  servo.write(servoAngle);
  delay(500);
}


void updateDisplay() {
  LCD.clear();


  LCD.setCursor(2, 0);
  LCD.print(dateTime);

  LCD.setCursor(0, 1);
  LCD.print("Alarm1 :  ");
  LCD.setCursor(10, 1);
  LCD.print(Alarm1);

  LCD.setCursor(0, 2);
  LCD.print("Alarm2 :  ");
  LCD.setCursor(10, 2);
  LCD.print(Alarm2);

  LCD.setCursor(0, 3);
  LCD.print("Alarm3 :  ");
  LCD.setCursor(10, 3);
  LCD.print(Alarm3);

  //delay(2000);
}
