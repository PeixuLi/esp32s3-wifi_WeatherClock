#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern struct tm localTime;

void sntp_get_time_task(void* parameter);
void Get_local_time(struct tm *localTime);

#ifdef __cplusplus
}
#endif