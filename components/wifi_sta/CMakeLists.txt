idf_component_register(SRCS "wifi_http_client.c" "wifi_mode_sta.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_wifi
                             esp_http_client
                             esp_netif
                             lwip
                             json
                             app_http_data)
