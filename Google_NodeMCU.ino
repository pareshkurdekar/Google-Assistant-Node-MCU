
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define Light   D7
#define Fan     D8

#define WLAN_SSID       "xxxxxxx"         // Replace 'x' with your SSID (Hotspot ID)
#define WLAN_PASS       "xxxxxxx"        // Replace 'x' with your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                                 // use 8883 for SSL
#define AIO_USERNAME    "pareshkurdekar"                     // Replace it with your username
#define AIO_KEY         "3c18505c292a4516af25ea29aaafd853"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/


WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'Light' and 'Fan' for subscribing to changes.
Adafruit_MQTT_Subscribe switch1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Light"); // Subscribe to data from the feed 'Light'
Adafruit_MQTT_Subscribe switch2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fan");  //Subscribe to data from the feed 'Fan'



void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(Light, OUTPUT);
  pinMode(Fan, OUTPUT);


  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // Setup MQTT subscription for swicth1, swicth2 feed.
  mqtt.subscribe(&switch1);
  mqtt.subscribe(&switch2);

  digitalWrite(D7,LOW);
  digitalWrite(D8,LOW);

}

void loop() {

  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &switch1) {                           // Sets the state of Light according to the data  received
      Serial.print(F("Got: "));
      Serial.println((char *)switch1.lastread);
      int State_1 = atoi((char *)switch1.lastread);
      digitalWrite(Light, State_1);

    }
    if (subscription == &switch2) {
      Serial.print(F("Got: "));                              // Sets the state of Fan according to the data  received
      Serial.println((char *)switch2.lastread);
      int State_2 = atoi((char *)switch2.lastread);
      digitalWrite(Fan, State_2);
    }

  }


}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");

}
