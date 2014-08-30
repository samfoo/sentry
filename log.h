#ifndef LOG_H
#define LOG_H

#include <string.h>
#include <stdio.h>

#define LOG(...) ({ char pmsg[256]; snprintf(pmsg, 256, __VA_ARGS__); Serial.println(pmsg); })
#define ERR(...) ({ char emsg[256]; snprintf(emsg, 256, __VA_ARGS__); Serial.println(emsg); })

#endif
