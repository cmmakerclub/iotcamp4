/******************************************************************************
  Project  : NECTEC IoT Camp 2016
  Board    : ESPresso Lite V2
  Author   : Chiang Mai Maker Club
*******************************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "CMMC_Blink.hpp"

CMMC_Blink blinker;

const char* ssid     = "";  // Change your ssid wifi
const char* password = "";  // Change your password wifi

// NETPIE.io : lab_device
#define APPID      ""       // Change your appID
#define KEY        ""       // Change your Key
#define SECRET     ""       // Change your SECRET
#define ALIAS      "thing1" // Change your name
#define CHAT_WITH  "thing2" // Change your chat with name

#define BUTTON 0

WiFiClient client;
MicroGear microgear(client);

void init_wifi();
void init_hardware() {
  pinMode(BUTTON, INPUT);
}

void setup() {
  init_wifi();
  init_hardware();
  Serial.println("Init done...");
}

void loop() {
  if (microgear.connected())
  {
    microgear.loop();

    bool state = digitalRead(BUTTON);
    if (state == 1) {
      microgear.chat(CHAT_WITH, "ON");
    }
    else {
      microgear.chat(CHAT_WITH, "OFF");
    }
    delay(200);
    
  }
  else
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
}

/******************* microgear loop ***********************************/
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName(ALIAS);
  microgear.subscribe("/gearname/#");
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String msg2 = String((char*)msg);
  String topic2 = String(topic);

  Serial.print(" Topic: ");
  Serial.print(topic2);
  Serial.print(" ---> ");
  Serial.println(msg2);
}

/******************* initial loop ***********************************/
void init_wifi() {
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  blinker.init();
  blinker.blink(50, LED_BUILTIN);
  Serial.begin(115200);
  delay(200);

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  blinker.blink(200, LED_BUILTIN);

  Serial.println("connecting netpie.io...");
  //microgear.resetToken();
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
  blinker.detach();
  Serial.println("netpie.io connected.");
}

