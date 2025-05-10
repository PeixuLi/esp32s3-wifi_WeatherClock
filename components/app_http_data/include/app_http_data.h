#ifndef __APP_HTTP_DATA_H_
#define __APP_HTTP_DATA_H_

typedef struct 
{
    char location[32];
    int  temperature;
    char weather_situation[16];
}Now_Weather_t;

#endif
