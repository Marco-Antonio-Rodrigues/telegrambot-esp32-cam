#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>
#include <queue>
#include "env.h"
#include "managerUsers.h"

extern std::queue<String> photoRequestQueue;

void configInitCamera();

void handleNewMessages(int numNewMessages);

void loop_poll_bot();

String sendPhotoTelegram(String chatId);

#endif
