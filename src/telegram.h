#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>
#include "env.h"

void configInitCamera();

void handleNewMessages(int numNewMessages);

String sendPhotoTelegram();

#endif
