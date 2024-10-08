//Arduino side code
//DHT11 Lib
#include <DHT.h>

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);

//Initialisation of DHT11 Sensor
// #define DHTPIN 4
// DHT dht(DHTPIN, DHT11);
float temp;
float hum;

void setup() {
  Serial.begin(9600);

  // dht.begin();
  nodemcu.begin(9600);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  StaticJsonDocument<1000> doc;

  //Obtain Temp and Hum data
  dht11_func();

  //Assign collected data to JSON Object
  doc["humidity"] = random(10);
  doc["temperature"] = random(10);
  doc["soil"] = random(10);
  doc["window"] = random(10);
  doc["waterPump"] = random(10);
  doc["humidifier"] = random(10);
  doc["seeder"] = random(10);
  doc["light"] = random(10);
  //Send data to NodeMCU
  serializeJson(doc, nodemcu);
  delay(2000);
}

void dht11_func() {

  // hum = dht.readHumidity();
  // temp = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp);

}