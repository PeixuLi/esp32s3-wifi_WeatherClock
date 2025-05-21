#ifndef __APP_MESSAGE_H_
#define __APP_MESSAGE_H_

typedef struct
{
    char text[128];
    uint32_t auto_close_delay;
}msgbox_request_t;

#endif