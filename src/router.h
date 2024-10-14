#ifndef MYSERVER_H
#define MYSERVER_H

#include <WebServer.h>
#include <WiFiManager.h>
#include "managerUsers.h"
#include "env.h"

extern WebServer server;
extern String validSessionID; 

void handleNotFound();

void handleRoot();

void setupServer();

void add_user();

void get_users();

void remove_user();

void handleLogin();

void handleLogout();

bool isAuthenticated();

#endif