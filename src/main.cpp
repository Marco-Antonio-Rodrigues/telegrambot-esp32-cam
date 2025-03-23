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
  
  // Desativa a proteção de brown-out
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Config and init the camera
  configInitCamera();

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
      s->set_hmirror(s, 1); // Espelha a imagem na horizontal
      s->set_vflip(s, 1);   // Espelha a imagem na vertical
  }


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
   }
   else{ //se não conectado na rede
      Serial.println("Conexão perdida, tentando reconectar...");
      wifiManager.autoConnect(wifiManager.getDefaultAPName().c_str(),"12345678");
   }
}