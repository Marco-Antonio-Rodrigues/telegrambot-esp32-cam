#ifndef ENV_H
#define ENV_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"


extern WiFiManager wifiManager;//Objeto de manipulação do wi-fi

// configurações do bot
extern String BOTtoken;

// Declarações para objetos globais
extern WiFiClientSecure clientTCP;
extern UniversalTelegramBot bot;

#define FLASH_LED_PIN 4
extern bool flashState;

extern bool modePresenceIsActive;
extern int pinoPIR; //PINO DIGITAL UTILIZADO PELO SENSOR
extern String chatId; //Chat para enviar as fotos quando o sensor está ativado. 


extern int botRequestDelay;
extern unsigned long lastTimeBotRan;

// Definições dos pinos da câmera
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void configInitCamera();

#endif
