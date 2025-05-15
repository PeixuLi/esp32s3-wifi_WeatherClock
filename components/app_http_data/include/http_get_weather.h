#ifndef __WIFI_HTTP_CLIENT_H_
#define __WIFI_HTTP_CLIENT_H_

typedef struct 
{
    char location[32];
    int  temperature;
    char weather_situation[16];
}Now_Weather_t;


void get_weather(void);
void Get_weather_task(void *pvParameters);

#endif 