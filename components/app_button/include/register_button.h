#ifndef __REGISTER_BUTTON_H_
#define __REGISTER_BUTTON_H_

extern button_handle_t btn1_handle, btn2_handle, btn3_handle;

void Register_Button(uint32_t button_num, button_handle_t* btn, button_cb_t button_event_cb);
void Register_Button_Event(button_handle_t* btn, button_event_t event, button_cb_t button_event_cb);
void Button_Init(void);

extern void button1_event_cb(void *arg, void *data);
extern void button2_event_cb(void *arg, void *data);
extern void button3_event_cb(void *arg, void *data);


#endif