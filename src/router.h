#ifndef MYSERVER_H
#define MYSERVER_H

#include <WebServer.h>

extern WebServer server;

void handleNotFound();

void handleRoot();

void setupServer();


#endif