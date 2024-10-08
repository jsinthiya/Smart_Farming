#include <ArduinoJson.h>
#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "WIFI SSID"
#define WIFI_PASSWORD "WIFI PASSWORD"

// Insert Firebase project API Key
#define API_KEY "API KEY"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "FIREBASE URL" 
 //MOTOR1 PINS nodeMCU
 int motorVertical_ena = D2;
 int in1 = D3;
 int in2 = D4;
 int in3 = D6;
 int in4 = D7;
 int motorLinear_enb = D5;
 int seederSevro=D8;

 Servo fishfeeder;
 Servo seederServo;

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

const size_t capacity = JSON_OBJECT_SIZE(10) + 40;

int fixedV=190;


void motorFront(int timePeriod,int speedV)
{
  //MOTOR_B COUNTERCLOCKWISE MAX SPEED-->FRONT
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(motorLinear_enb, speedV);
 
  delay(timePeriod);


  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(250);

}

void motorBack(int timePeriod,int speedV)
{
  
  //MOTOR_B CLOCKWISE MAX SPEED-->BACK
   digitalWrite(in3,LOW);
   digitalWrite(in4,HIGH);
   analogWrite(motorLinear_enb, speedV);
  delay(timePeriod);
 
  digitalWrite(in3,LOW);
   digitalWrite(in4,LOW);
  delay(250);


}

void motorUp(int timePeriod,int speedV)
{
   //MOTOR_A COUNTERCLOCKWISE MAX SPEED-->UP
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  analogWrite(  motorVertical_ena, speedV);
  
  delay(timePeriod);
    //STOP
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
    delay(500);
}
void motorDown(int timePeriod,int speedV)
{
  // MOTOR_A CLOCKWISE MAX SPEED-->DOWN
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite( motorVertical_ena, speedV);
    delay(timePeriod);
     
  //STOP
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  delay(500);
}

void seederRun()
{
  motorDown(1500,240);
  motorFront(4500,160);
  motorBack(1625,160);
  delay(2000);

  motorBack(1625,fixedV);
  delay(2000);
  motorBack(1625,fixedV);
  delay(2000);

  motorBack(1625,fixedV);
  delay(2000);

  motorUp(1700,240);

}



void setup() {

  pinMode( motorVertical_ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(motorLinear_enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("firebase connection estublish");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  while (!Serial) continue;
}
int backV;
void loop() {

  if (Firebase.ready() && signupOK){

  if (Firebase.RTDB.getBool(&fbdo, "/greenhouse/seeder")) {
   
      bool seederStatus = fbdo.boolData();
      Serial.println(seederStatus);
      if(seederStatus)
      {
        seeder();
        //seeder function
      }
    }
   else {
    Serial.println("Failed to get seeder data from Firebase");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.getBool(&fbdo, "/pond")) {
   
      bool pondStatus = fbdo.boolData();
      if(pondStatus)
      {
          fishFeed(180);
          // delay(2000);
          // fishFeed(0);
        //fish feeder on
      }
      else
      {
        fishFeed(0);
      }
      Serial.println(pondStatus);
    }
   else {
    Serial.println("Failed to get seeder data from Firebase");
    Serial.println(fbdo.errorReason());
  }



  DynamicJsonDocument doc(capacity);
  while(Serial.available())
  {
    deserializeJson(doc, Serial);
  }
  serializeJson(doc, Serial);
  Serial.println();
  if (doc.containsKey("greenHumidity") && doc.containsKey("greentemperature")) {
    // Serial.println("JSON Object Received");
    // float hum = doc["greenHumidity"];
    // float temp = doc["greentemperature"];

    // Serial.print("Received Humidity: ");
    // Serial.println(hum);
    // Serial.print("Received Temperature: ");
    // Serial.println(temp);
    // Serial.println("-----------------------------------------");

    Firebase.RTDB.setFloat(&fbdo, "greenhouse/humidity", doc["greenHumidity"]);
    Firebase.RTDB.setFloat(&fbdo, "greenhouse/temperature", doc["greentemperature"]);
    Firebase.RTDB.setFloat(&fbdo, "greenhouse/soil", doc["greensoil"]);
    Firebase.RTDB.setFloat(&fbdo, "greenhouse/lightSensor", doc["greenlight"]);
    Firebase.RTDB.setFloat(&fbdo, "greenhouse/windowOpener", doc["greenWindow"]);

    Firebase.RTDB.setFloat(&fbdo, "incubator/humidity", doc["incubatorHumidity"]);
    Firebase.RTDB.setFloat(&fbdo, "incubator/temperature", doc["incubatorTemperature"]);
    Firebase.RTDB.setFloat(&fbdo, "incubator/turnner", doc["incubatorTurnner"]);
    Firebase.RTDB.setFloat(&fbdo, "incubator/light", doc["incubatorLight"]);




    // Serial.println("Arduino 2 Data Uploaded");
  
  }

  }
  delay(2000);
}
void seeder()
{
   fixedV=200;
   backV=180;
  motorDown(1300,200);
  motorFront(3900,180);

  motorBack(800,backV);
  delay(2000);
  
  motorBack(800,backV);
  delay(2000);
  motorBack(800,backV);
  delay(2000);
  motorBack(800,backV);
  delay(2000);
  motorUp(1500,200);
}




void fishFeed(int targetAngle) {
        fishfeeder.attach(D1);
  int currentAngle = fishfeeder.read();
  if (currentAngle < targetAngle) {
    for (int i = currentAngle; i <= targetAngle; i++) {
      fishfeeder.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  } else if (currentAngle > targetAngle) {
    for (int i = currentAngle; i >= targetAngle; i--) {
      fishfeeder.write(i);
      delay(10); // Adjust the delay for smoother movement
    }
  }
  fishfeeder.detach();
}




