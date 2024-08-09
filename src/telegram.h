#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>
#include "env.h"

void configInitCamera();

void handleNewMessages(int numNewMessages);

void loop_poll_bot();

String sendPhotoTelegram();

#endif
