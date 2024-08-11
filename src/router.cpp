#include "router.h"

WebServer server(80); 

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
      server.send(404, "text/plain", "Arquivo não encontrado");
      return;
  }
  
  // Envia o conteúdo do arquivo HTML
  server.streamFile(file, "text/html");
  file.close();
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

void setupServer(){
  server.on("/", handleRoot);
   // Rota para adicionar um ID de usuário
    server.on("/add_user", HTTP_POST, []() {
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
    });

    // Rota para obter todos os IDs de usuários
    server.on("/get_users", HTTP_GET, []() {
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
    });

    // Rota para remover um ID de usuário
    server.on("/remove_user", HTTP_POST, []() {
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
    });

  server.onNotFound(handleNotFound);
}