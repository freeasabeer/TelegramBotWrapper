#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "TelegramBotWrapper.h"


// Initialize Telegram BOT
#define BOTtoken "your Bot Token (Get from Botfather)"

// To get chat id: send a message to the bot and then browse to:
// https://api.telegram.org/<BOTtoken>/getUpdates
// You will find the chat id in the answer
#define CHAT_ID "1234567890"

WiFiClientSecure client;
TelegramBotWrapper telegram(BOTtoken, client, CHAT_ID);

void handle_menu(cb_param_t *ptr)
{
  String keyboardJson = "[[\"/item1\", \"/item2\"], [\"/item3\"]]";
  telegram.sendKeyboard((ptr)?ptr->chat_id:"", "Available options", keyboardJson);
}

void handle_item1(cb_param_t *ptr) {
  // handle /menu1 (and check command parameter for a given value
  const char *param = (ptr)?ptr->param:NULL;

  if (param) {
    if (strcmp(param, "param1")==0) {
      // Handle /item1 param1
      Serial.println("Got /item1 param1");
    } else {
      // handle /item1 <whatever param was sent>
      Serial.printf("Got /item1 %s\n", param);
    }
    telegram.send((ptr)?ptr->chat_id:"", "Ok");
  }
}

void handle_item2(cb_param_t *ptr) {
  // handle /item2 (and don't care of any parameter following the command)
  Serial.println("Got /item2");
  telegram.send((ptr)?ptr->chat_id:"", "Ok");
}

void handle_item3(cb_param_t *ptr) {
  // handle /item3 (and don't care of any parameter following the command)
  Serial.println("Got /item3");
  telegram.send((ptr)?ptr->chat_id:"", "Ok");
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\n");

  Serial.printf("Connecting to Wi-Fi...\n");
  Wifi.begin("Your SSID", "Your secret key");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  // If we reached that point, then we got connected to an access point as a client !
  Serial.println("WiFi connected");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

  telegram.send("", String("WiFi connected\nIP address: ")+WiFi.localIP().toString());

  telegram_cmd_cb_t telegram_cmd_cb[] {
    {"/item1",   &handle_item1},
    {"/item2",   &handle_item2},
    {"/item3",   &handle_item3},
    {"/menu",    &handle_menu},
    {nullptr,    nullptr}
  };
  telegram.register_bundle(telegram_cmd_cb);

  handle_menu(NULL);
}

void loop(void) {
  telegram.handle();
}