#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern struct tm localTime;

int sntp_get_time(void);
void Get_local_time(struct tm *localTime);

#ifdef __cplusplus
}
#endif