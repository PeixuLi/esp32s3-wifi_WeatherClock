idf_component_register(SRCS "app_http_data.c" "http_get_weather.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client
                             esp_wifi
                             esp_netif
                             lwip
                             json
                             app_http_data)
