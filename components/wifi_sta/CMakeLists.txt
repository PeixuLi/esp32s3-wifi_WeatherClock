idf_component_register(SRCS "wifi_sntp_get.c" "wifi_mode_sta.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_wifi
                             esp_netif
                             lwip
                             app_http_data
                             app_ui)
