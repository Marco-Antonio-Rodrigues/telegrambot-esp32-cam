#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include "router.h"
#include "env.h" 
#include "telegram.h"

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
  Serial.println();
    
  // Desativa a proteção de brown-out
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Config and init the camera
  configInitCamera();

  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 

  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  wifiManager.autoConnect();
  if (WiFi.status()!= WL_CONNECTED) {
    Serial.println("Abertura Portal"); //Abre o portal
    wifiManager.resetSettings();       //Apaga rede salva anteriormente
    if(!wifiManager.startConfigPortal("ESP32-CONFIG", "12345678") ){ //Nome da Rede e Senha gerada pela ESP
      Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
      delay(2000);
      ESP.restart(); //Reinicia ESP após não conseguir conexão na rede
    }
    else{       //Se caso conectar 
      Serial.println("Conectado na Rede!!!");
      ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
    }
  }
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  setupServer();
  server.begin();
}
 
void loop() {
   if(WiFi.status()== WL_CONNECTED){ //Se conectado na rede
      // Serial.println("Conectado");
   }
   else{ //se não conectado na rede
      Serial.println("Conexão perdida, tentando reconectar...");
      wifiManager.autoConnect();//Função para se autoconectar na rede
   }
   server.handleClient();
   loop_poll_bot();
}