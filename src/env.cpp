#include <ArduinoJson.h>
#include "env.h"

const char* ssid = "cole_aqui";
const char* password = "cole_aqui";
String BOTtoken = "cole_aqui";
String CHAT_ID = "cole_aqui";

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

bool sendPhoto = false;

bool flashState = LOW;

int botRequestDelay = 1000;
unsigned long lastTimeBotRan = 0;
