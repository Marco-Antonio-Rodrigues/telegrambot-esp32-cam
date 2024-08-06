#include "webserver.h"

AsyncWebServer server(80); 

void setupServer() {
	 server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { 
	   Serial.println("ESP32 Web Server: New request received:");  // for debugging 
	   Serial.println("GET /");        // for debugging 
	   request->send(200, "text/html", "<html><body><h1>Hello, ESP32!</h1></body></html>"); 
	}); 
} 
