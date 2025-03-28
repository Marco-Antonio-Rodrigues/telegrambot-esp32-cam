#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include "router.h"
#include "env.h" 
#include "telegram.h"
#include "managerUsers.h"


WiFiManager wifiManager;//Objeto de manipulação do wi-fi
String header;

unsigned long lastReconnectAttempt = 0; 

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}
 
//Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}

void setup(){
  Serial.begin(115200);
  pinMode(FLASH_LED_PIN, OUTPUT); 
    
  initLittleFS();//init managerFile

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  

  // Config and init the camera
  configInitCamera();

  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 

  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  wifiManager.autoConnect(wifiManager.getDefaultAPName().c_str(),"12345678");

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  setupServer();
  server.begin();
}
 
void loop() {
  if(WiFi.status()== WL_CONNECTED){ //Se conectado na rede
    server.handleClient();
    loop_poll_bot();
  }else if (millis() - lastReconnectAttempt > 10000) { // Tenta reconectar a cada 10s
    Serial.println("Conexão perdida, tentando reconectar..."); 
    // Obtém SSID e senha salvos do WiFiManager
    String ssid = wifiManager.getWiFiSSID();
    String password = wifiManager.getWiFiPass();
    
    WiFi.begin(ssid.c_str(), password.c_str()); 
    
    lastReconnectAttempt = millis();
    if (WiFi.status()== WL_CONNECTED){
      Serial.println("Conexão restabelecida!"); 
    }
  }
}

