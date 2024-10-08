//Arduino side code
//DHT11 Lib
#include <DHT.h>

//Arduino to NodeMCU Lib
#include <ArduinoJson.h>
#include "DHT.h"
#include <Servo.h>
#include <ArduinoJson.h>
//For DHT Temperature & Humidity Sensor
#define DHTPIN 2
#define HUMIDITYFIERPIN 4
#define SOILPIN A0
#define LIGHTSENSORPIN A1
#define DHTTYPE DHT11
#define DHT11_2 13
#define EGGTURNER 12
const size_t capacity = JSON_OBJECT_SIZE(20) + 40;

DHT dht(DHTPIN, DHTTYPE);
Servo windowMotor;
Servo eggTurrner;
//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)

//Initialisation of DHT11 Sensor

struct SoilSensor {
  int snesorValue;
  int level;
};
struct DHT11Sensor {
  float humidity;
  float temperature;
};
struct LightSensor {
  int sensorValue;
  int level;
};
LightSensor lightLevel() {
  int light = analogRead(LIGHTSENSORPIN);
  int level;
  if (light < 500) {
    level = 0;
  } else {
    level = 1;
  }

  return { light, level };
}
bool humidifier(LightSensor lightData, DHT11Sensor dht11data) {
  bool status = false;
  if (lightData.level == 0 && dht11data.humidity < 60) {
    status = true;
    digitalWrite(HUMIDITYFIERPIN, LOW);
  } else if (lightData.level == 1 && dht11data.humidity <= 80) {
    status = true;
    digitalWrite(HUMIDITYFIERPIN, LOW);
  } else {
    status = false;
    digitalWrite(HUMIDITYFIERPIN, HIGH);
  }
  return status;
}
DHT11Sensor tempAndHumidity() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    return { NULL, NULL };
  }

  return { humidity, temperature };
}
//For Soil Mosture Sensor
SoilSensor soilMosture() {
  int soilMosture = analogRead(SOILPIN);
  int level;
  if (soilMosture < 300) {
    level = 1;
  } else if (soilMosture > 300 && soilMosture < 950) {
    level = 2;
  } else if (soilMosture > 950) {
    level = 0;
  }

  return { soilMosture, level };
}

void setup() {
  Serial.begin(9600);


}

void loop() {

DynamicJsonDocument doc(capacity);

  DHT11Sensor dht11data = tempAndHumidity();
  SoilSensor soilData = soilMosture();
  LightSensor lightData = lightLevel();
  //Assign collected data to JSON Object


  bool window = false;
  bool humidifierStatus = humidifier(lightData, dht11data);
  if (lightData.level==1)
  {
    window = true;
  
    Serial.println(F(" Humidifier ON"));
    //digitalWrite(PUMP_PIN, HIGH);

    moveServo(0);
  }
  else {

    window = false;
    moveServo(90); // Turn on servo if light is dim
  }
  Serial.println();



  doc["greenHumidity"] = dht11data.humidity;
  doc["greentemperature"] = dht11data.temperature;
  doc["greensoil"] = soilData.snesorValue;
  doc["greenlight"] = lightData.sensorValue;
  doc["greenWindow"] = false;
  

  doc["incubatorHumidity"] = 0;
  doc["incubatorTemperature"] = 0;
  doc["incubatorTurnner"] = 1;
  doc["incubatorLight"] = 1;

  doc["incubatorLight"] = 1;
    //Send data to NodeMCU
  serializeJson(doc, Serial);
  Serial.println();
   eggTurn(45);
   eggTurn(0);
  delay(2000);
}
void moveServo(int targetAngle) {
        windowMotor.attach(9);
  int currentAngle = windowMotor.read();
  if (currentAngle < targetAngle) {
    for (int i = currentAngle; i <= targetAngle; i++) {
      windowMotor.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  } else if (currentAngle > targetAngle) {
    for (int i = currentAngle; i >= targetAngle; i--) {
      windowMotor.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  }
  windowMotor.detach();
}

void eggTurn(int targetAngle) {
        eggTurrner.attach(EGGTURNER);
  int currentAngle = eggTurrner.read();
  if (currentAngle < targetAngle) {
    for (int i = currentAngle; i <= targetAngle; i++) {
      eggTurrner.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  } else if (currentAngle > targetAngle) {
    for (int i = currentAngle; i >= targetAngle; i--) {
      eggTurrner.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  }
  eggTurrner.detach();
}

