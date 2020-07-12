#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "MFJ_ZMI"
#define WIFI_PASS "456dfmncgh"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "faishal94"
#define MQTT_PASS "062ec0cf04b14376b9bc4843aa7b9623"

Servo servo;

int led1 = 13;
int led2 = 12;
int led3 = 14;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe goingup = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/GoingUp");
Adafruit_MQTT_Subscribe firstfloor = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/FirstFloor");
Adafruit_MQTT_Subscribe goingdown = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/GoingDown");
Adafruit_MQTT_Publish LightsStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/LightsStatus");

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  //Initialize Servo
  servo.attach(2); //D4
  servo.write(90);
  
  delay(2000);
  
  //Initialize the LCD 16x2
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("WELCOME");
  delay(2500);
  lcd.setCursor(1,1);
  lcd.print("Say Hey, Siri!");
  
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  digitalWrite(LED_BUILTIN, LOW);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }

  Serial.println("OK!");

  //Subscribe to the onoff topic
  
  mqtt.subscribe(&goingup);
  mqtt.subscribe(&firstfloor);
  mqtt.subscribe(&goingdown);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

}

void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    
    if (subscription == &goingup)
    {
      //Print the new value to the serial monitor
      Serial.print("GoingUp: ");
      Serial.println((char*) goingup.lastread);
      digitalWrite(led1, HIGH);
      servo.write(0);
      delay(5000);
      LightsStatus.publish("Going Up from Ground");

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) goingup.lastread, "First"))
      {
        digitalWrite(led1, LOW);
        delay(5000);
        LightsStatus.publish("First");
        digitalWrite(led2, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("FIRST FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led1, LOW);
        digitalWrite(led3, LOW);
        delay(5000);
        servo.write(90);
      }
      else if (!strcmp((char*) goingup.lastread, "Second"))
      {
        digitalWrite(led1, LOW);
        delay(5000);
        LightsStatus.publish("Second");
        digitalWrite(led3, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("SECOND FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        delay(5000);
        servo.write(90);
      }
      else
      {
        LightsStatus.publish("ERROR");
      }
    }
    if (subscription == &firstfloor)
    {
      //Print the new value to the serial monitor
      Serial.print("FirstFloor: ");
      Serial.println((char*) firstfloor.lastread);
      digitalWrite(led2, HIGH);
      servo.write(0);
      delay(5000);
      LightsStatus.publish("Going up or down from first floor");

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) firstfloor.lastread, "Second"))
      {
        digitalWrite(led2, LOW);
        delay(5000);
        LightsStatus.publish("Second");
        digitalWrite(led3, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("SECOND FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        delay(5000);
        servo.write(90);
      }
      else if (!strcmp((char*) firstfloor.lastread, "Ground"))
      {
        digitalWrite(led2, LOW);
        delay(5000);
        LightsStatus.publish("Ground");
        digitalWrite(led1, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("GROUND FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        delay(5000);
        servo.write(90);
      }
      else
      {
        LightsStatus.publish("ERROR");
      }
    }
    if (subscription == &goingdown)
    {
      //Print the new value to the serial monitor
      Serial.print("GoingDown: ");
      Serial.println((char*) goingdown.lastread);
      digitalWrite(led3, HIGH);
      servo.write(0);
      delay(5000);
      LightsStatus.publish("Going down from second floor");

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) goingdown.lastread, "First"))
      {
        digitalWrite(led3, LOW);
        delay(5000);
        LightsStatus.publish("First");
        digitalWrite(led2, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("FIRST FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led1, LOW);
        digitalWrite(led3, LOW);
        delay(5000);
        servo.write(90);
      }
      else if (!strcmp((char*) goingdown.lastread, "Ground"))
      {
        digitalWrite(led3, LOW);
        delay(5000);
        LightsStatus.publish("Ground");
        digitalWrite(led1, HIGH);
        servo.write(90);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YOU ARRIVED AT");
        delay(2000);
        lcd.setCursor(0,1);
        lcd.print("GROUND FLOOR");
        delay(2000);
        servo.write(0);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        delay(5000);
        servo.write(90);
      }
      else
      {
        LightsStatus.publish("ERROR");
      }
    }
    else
    {
      //LightsStatus.publish("ERROR");
    }
  }
  //  if (!mqtt.ping())
  //  {
  //    mqtt.disconnect();
  //  }
}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
