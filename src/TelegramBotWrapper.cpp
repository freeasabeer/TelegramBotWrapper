#include <String>
#include "TelegramBotWrapper.h"

TelegramBotWrapper::TelegramBotWrapper(const String &token, WiFiClientSecure &client, const String &chat_id):UniversalTelegramBot(token, client)
  {
    this->_chat_id = chat_id;
    this->_client = &client;
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  }

// Private functions
void TelegramBotWrapper::handleNewMessages(int numNewMessages)
{
//  Serial.println("handleNewMessages");
//  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    bool handle_found = false;
    const char *text = this->messages[i].text.c_str();
    const char *chat_id = this->messages[i].chat_id.c_str();
    Serial.printf("chat id: %s\n", chat_id);
    Serial.printf("message: %s\n", text);

    int j = 0;
    if (text[0] == '/') {
      //Serial.println("Got a /");
      char *pos = strchr(text, ' ');
      if (pos) {
        *pos = 0;
        pos++;
        //Serial.printf("Got a command: '%s' with a parameter: '%s'\n", text, pos);
      } else {
        //Serial.printf("Got a command: '%s' without any parameter\n", text);
      }
      while(this->telegram_cmd_cb[j] != nullptr) {
        if(strcmp(text, this->telegram_cmd_cb[j]->cmd) == 0) {
          void (*cb)(cb_param_t *param) = this->telegram_cmd_cb[j]->cb;
          //Serial.printf("call back #%d is matching: calling 0x%08X\n", j, (unsigned int)cb);
          cb_param_t cb_param;
          cb_param.chat_id = chat_id;
          cb_param.param = pos;
          (*cb)(&cb_param);
          handle_found =true;
        }
        j++;
      }
      if (!handle_found)
        Serial.println("No handle found");
    }
  }
}

// public functions

void TelegramBotWrapper::set_mtbs(unsigned long mtbs){
  this->_mtbs = mtbs;
}

bool TelegramBotWrapper::register_single(const char *cmd, void (*cb)(cb_param_t *param))
{
  static int nb_cb = 0;
  telegram_cmd_cb_t *ptr;

  if (nb_cb == 0)
    this->telegram_cmd_cb = (telegram_cmd_cb_t **)malloc(2*sizeof(telegram_cmd_cb_t *));
  else
    this->telegram_cmd_cb = (telegram_cmd_cb_t **)realloc(this->telegram_cmd_cb, (nb_cb+2)*sizeof(telegram_cmd_cb_t *));

  if(!this->telegram_cmd_cb)
    return false;

  ptr = (telegram_cmd_cb_t *)malloc(sizeof(telegram_cmd_cb_t));

  if(!ptr)
    return false;

  this->telegram_cmd_cb[nb_cb] = ptr;
  this->telegram_cmd_cb[nb_cb+1] = nullptr;

  ptr->cmd = cmd;
  ptr->cb = cb;

  //Serial.printf("[%d] cmd: %s -> cb: 0x%08X\n", nb_cb, this->telegram_cmd_cb[nb_cb]->cmd, (unsigned int)this->telegram_cmd_cb[nb_cb]->cb);

  nb_cb++;

  return true;
}

/*
void TelegramBotWrapper::check() {
    int j = 0;
      while(this->telegram_cmd_cb[j] != nullptr) {
        Serial.printf("[%d] cmd: %s -> cb: 0x%08X\n", j, this->telegram_cmd_cb[j]->cmd, (unsigned int)this->telegram_cmd_cb[j]->cb);
        j++;
    }
}
*/

bool TelegramBotWrapper::register_bundle(telegram_cmd_cb_t *ptr) {
  bool status = true;

  if (!ptr)
    return false;

  while (ptr->cmd != nullptr) {
    status = status && register_single(ptr->cmd, ptr->cb);
    ptr++;
  }
  return status;
}

void TelegramBotWrapper::handle(void) {
  static bool firstTime = true;
  static unsigned long bot_lasttime = 0;
  if (millis() - bot_lasttime > this->_mtbs) {
    int numNewMessages = this->getUpdates(this->last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      // do not process all pending messages sent after we've been switched off and before we've been switched on again
      if (!firstTime)
        handleNewMessages(numNewMessages);
      numNewMessages = this->getUpdates(this->last_message_received + 1);
    }
    bot_lasttime = millis();
    if (firstTime)
      firstTime = false;
  }
}

String TelegramBotWrapper::get_chat_id(void) {
  return this->_chat_id;
}

void TelegramBotWrapper::set_chat_id(const String &chat_id) {
    this->_chat_id = chat_id;
}

void TelegramBotWrapper::send(const String &chat_id, const String &message) {
  if (chat_id == "")
    this->sendMessage(this->_chat_id, message, "");
  else
    this->sendMessage(chat_id, message, "");
}

void TelegramBotWrapper::sendKeyboard(const String &chat_id, const String &text, const String &keyboard) {
  if (chat_id == "")
    this->sendMessageWithReplyKeyboard(this->_chat_id, text, "", keyboard, true, false, false);
  else
    this->sendMessageWithReplyKeyboard(chat_id, text, "", keyboard, true, false, false);
}