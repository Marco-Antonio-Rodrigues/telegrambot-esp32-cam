#include "telegram.h"

std::queue<String> photoRequestQueue;

String sendPhotoTelegram(String chatId) {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  //Dispose first picture because of bad quality
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb); // dispose the buffered image
  
  // Take a new photo
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));


  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    size_t imageLen = fb->len;
    size_t extraLen = head.length() + tail.length();
    size_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);

    bool authorized = false;

    File file = LittleFS.open(usersFile, "r");
    if (!file) {
        bot.sendMessage(chat_id, "Erro ao abrir arquivo", "");
        return;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();  // Remove espaços em branco e quebras de linha

        if (line == chat_id) {
            authorized = true;
            break;
        }
    }
    file.close();

    if (!authorized) {
        bot.sendMessage(chat_id, "Usuário não autorizado", "");
        continue;
    }

    String text = bot.messages[i].text;
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use os seguinte comandos para interagir com o ESP32-CAM \n";
      welcome += "/photo : Tirar um foto\n";
      welcome += "/flash : Alterar estado do LED\n";
      welcome += "/presence : Alterar estado do sensor de presença\n";
      welcome += "\nAcesse o admin em:\n";
      welcome += "opção 1: http://esp32.local\n";
      welcome += "opção 2: http://192.168.0.102\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Change flash LED state");
    }
    if (text == "/photo") {
        photoRequestQueue.push(chat_id);
        Serial.println("New photo request from: "+ chat_id);
    }
    if (text == "/presence") {
        modePresenceIsActive = !modePresenceIsActive;
        Serial.println("Change mode presence state");
    }
    
  }
}

void loop_poll_bot(){
  if (!photoRequestQueue.empty()) {
      Serial.println("Preparing photo for: " + photoRequestQueue.front());
      sendPhotoTelegram(photoRequestQueue.front());
      photoRequestQueue.pop();
  }
  if (modePresenceIsActive && (digitalRead(pinoPIR) == HIGH) && photoRequestQueue.size() < 5) {
      photoRequestQueue.push(chatId);
  }
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}