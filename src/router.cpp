#include "router.h"

WebServer server(80); 
String validSessionID = "eb3b564b7150aa15ba8351c7365a90c9"; 

bool isAuthenticated() {
    if (server.hasHeader("Cookie")) {
        String cookie = server.header("Cookie");
        Serial.println("cookie extraído de header: " + cookie);
        if (cookie.indexOf("sessionId="+validSessionID) != -1) {
            Serial.println("Authentication Successful");
            return true;
        }
    }
    return false;
}

void handleRoot() {
    if (isAuthenticated()) {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            server.send(404, "text/plain", "Arquivo não encontrado");
            return;
        }
        
        // Envia o conteúdo do arquivo HTML
        server.streamFile(file, "text/html");
        file.close();
    }else{
        server.send(403, "text/plain", "Acesso negado");
    }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void add_user(){
    if (isAuthenticated()){
        if (server.hasArg("id")) {
            String userId = server.arg("id");

            // Adiciona o ID ao arquivo de usuários
            File file = LittleFS.open(usersFile, "a");
            if (!file) {
                server.send(500, "text/plain", "Erro ao abrir arquivo");
                return;
            }
            file.println(userId);
            file.close();

            server.send(200, "text/plain", "ID Adicionado");
        } else {
            server.send(400, "text/plain", "ID não recebido");
        }
    }else{
        server.send(403, "text/plain", "Acesso negado");
    }
}

void get_users(){
    if(isAuthenticated()){
        File file = LittleFS.open(usersFile, "r");
            if (!file) {
                server.send(500, "text/plain", "Erro ao abrir arquivo");
                return;
            }

            String jsonResponse = "[";
            String line;
            while (file.available()) {
                line = file.readStringUntil('\n');
                line.trim();
                if (line.length() > 0) {
                    if (jsonResponse.length() > 1) {
                        jsonResponse += ",";
                    }
                    jsonResponse += "\"" + line + "\"";
                }
            }
            jsonResponse += "]";
            file.close();

            server.send(200, "application/json", jsonResponse);
    }else{
        server.send(403, "text/plain", "Acesso negado");
    }
}

void remove_user(){
    if(isAuthenticated()){
        if (server.hasArg("id")) {
            String userId = server.arg("id");
            File file = LittleFS.open(usersFile, "r");
            if (!file) {
                server.send(500, "text/plain", "Erro ao abrir arquivo");
                return;
            }

            // Cria um arquivo temporário para reescrever os IDs sem o ID removido
            File tempFile = LittleFS.open("/temp.txt", "w");
            if (!tempFile) {
                server.send(500, "text/plain", "Erro ao abrir arquivo temporário");
                return;
            }

            String line;
            bool found = false;
            while (file.available()) {
                line = file.readStringUntil('\n');
                line.trim();
                if (line != userId) {
                    tempFile.println(line);
                } else {
                    found = true;
                }
            }
            file.close();
            tempFile.close();

            // Substitui o arquivo original pelo temporário
            LittleFS.remove(usersFile);
            LittleFS.rename("/temp.txt", usersFile);

            if (found) {
                server.send(200, "text/plain", "ID Removido");
            } else {
                server.send(404, "text/plain", "ID não encontrado");
            }
        } else {
            server.send(400, "text/plain", "ID não recebido");
        }
    }else{
        server.send(403, "text/plain", "Acesso negado");
    }
}

void handleLogin() {
    if (server.method() == HTTP_GET) {
        // Se for uma requisição GET, exiba o formulário de login
        server.send(200, "text/html", 
            "<form action='/login' method='POST'>"
            "Usuário: <input type='text' name='username'><br>"
            "Senha: <input type='password' name='password'><br>"
            "<input type='submit' value='Login'></form>");
    } else if (server.method() == HTTP_POST) {
        if (server.hasArg("username") && server.hasArg("password")) {
            String username = server.arg("username");
            String password = server.arg("password");
            
            if (username == wifiManager.getWiFiSSID() && password == wifiManager.getWiFiPass()) {
                server.sendHeader("Cache-Control", "no-cache");
                server.sendHeader("Set-Cookie", "sessionId=" + validSessionID + "; HttpOnly; SameSite=Strict; Max-Age=31536000");
                server.sendHeader("Location", "/", true);
                server.send(301);
            } else {
                server.send(403, "text/html", "Credenciais inválidas");
            }
        } else {
            server.send(400, "text/html", "Faltando parâmetros");
        }
    }else {
        server.send(405, "text/html", "Método não permitido");
    }
}

void handleLogout() {
    server.sendHeader("Set-Cookie", "sessionId=; HttpOnly; SameSite=Strict; Max-Age=0");  // Remove o cookie
    server.sendHeader("Location", "/login", true);
    server.send(301);
}

void setupServer(){
    server.on("/", handleRoot);
    server.on("/login", handleLogin);
    server.on("/logout",HTTP_POST, handleLogout);
    server.on("/get_users", HTTP_GET,get_users);
    server.on("/add_user", HTTP_POST, add_user);
    server.on("/remove_user", HTTP_POST,remove_user);
    server.onNotFound(handleNotFound);

    //here the list of headers to be recorded
    const char *headerkeys[] = {"User-Agent", "Cookie"};
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
    //ask server to track these headers
    server.collectHeaders(headerkeys, headerkeyssize);
}