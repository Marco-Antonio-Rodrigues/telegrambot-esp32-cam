#ifndef MANAGERUSERS_H
#define MANAGERUSERS_H

#include <LittleFS.h>

extern const char* usersFile;

void initLittleFS();

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path);
// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message);

#endif