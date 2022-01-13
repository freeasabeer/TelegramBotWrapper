#ifndef _TelegramBotWrapper_H
#define _TelegramBotWrapper_H

#include <String.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

typedef struct {
    const char *chat_id;
    const char *param;
} cb_param_t;

typedef struct {
  const char * cmd;
  void (*cb)(cb_param_t *param);
} telegram_cmd_cb_t;

class TelegramBotWrapper : public UniversalTelegramBot
{
public:
  using UniversalTelegramBot::UniversalTelegramBot;
  void begin(WiFiClientSecure &client, const String &chat_id);
  void send(const String &chat_id, const String &message);
  void handle(void);
  bool register_single(const char *cmd, void (*cb)(cb_param_t *param));
  bool register_bundle(telegram_cmd_cb_t *ptr);
  UniversalTelegramBot *get_bot(void);
  void set_chat_id(const String &chat_id);
  String *get_chat_id(void);
  void sendKeyboard(const String &chat_id, const String &text, const String &keyboard);
  void set_mtbs(unsigned long mtbs);

private:
  //WiFiClientSecure _client;
  //UniversalTelegramBot *_bot;
  telegram_cmd_cb_t **telegram_cmd_cb = nullptr;
  String _chat_id;
  unsigned long _mtbs = 5000; // mean time between scan messages

  void handleNewMessages(int numNewMessages);
  //void check(void);
};
#endif /* _TelegramBotWrapper_H */
