#ifdef DEBUG
#define LOG(...) ({ char pmsg[256]; snprintf(pmsg, 256, __VA_ARGS__); Serial.println(pmsg); })
#else
#define LOG(X)
#endif

#define ERR(...) ({ char emsg[256]; snprintf(emsg, 256, __VA_ARGS__); Serial.println(emsg); })

