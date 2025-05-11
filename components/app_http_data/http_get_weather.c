#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "app_http_data.h"
#include "http_get_weather.h"

//url: <协议>://<主机名>:<端口>/<路径>?<查询字符串>
//https://api.seniverse.com/v3/weather/now.json?key=SsRFsgFEzReo_TlLM&location=zhuhai&language=zh-Hans&unit=c

static const char *TAG = "http_client.c";

#define API_KEY     "SsRFsgFEzReo_TlLM"
#define LOCATION    "zhuhai"

Now_Weather_t now_weather;

esp_http_client_handle_t http_client_handle;
#define MAX_HTTP_BUFFER 2048

static void parsed_json(const char *buffer);

esp_err_t http_event_handler_cb(esp_http_client_event_t *evt)
{
    static char *output_buffer = NULL;
    static int output_len = 0;

    //解析 HTTP 状态码（200 表示成功，404 表示未找到，等等）。
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGW("http_event_handler_cb", "HTTP_EVENT_ERROR");
        
        break;

    case HTTP_EVENT_ON_CONNECTED: 
        ESP_LOGD("http_event_handler_cb", "HTTP_EVENT_ON_CONNECTED");
        break;

    case HTTP_EVENT_ON_DATA:// 接收数据事件
        ESP_LOGI("http_event_handler_cb", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

        //检查 HTTP 响应是否为分块传输编码
        if (!esp_http_client_is_chunked_response(evt->client)) 
        {
            //printf("%.*s\r\n", evt->data_len, (char *)evt->data); //原始json数据
        ////////////////////////////////////////////////////////////////////////        
            //初始化缓冲区，为存储响应数据分配缓冲区（如果尚未分配
            if (output_buffer == NULL) 
            {
                output_buffer = (char *)malloc(MAX_HTTP_BUFFER);
                if (output_buffer == NULL) {
                    ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                    return ESP_FAIL;
                }
                output_len = 0;
            }
        ////////////////////////////////////////////////////////////////////////
            //将接收到的数据存到数组中，等待解析
            if (output_len + evt->data_len < MAX_HTTP_BUFFER)
            {
                //output_len:偏移量
                memcpy(output_buffer+output_len, evt->data, evt->data_len);
                output_len += evt->data_len;//更新 output_len，累加本次接收的数据长度
            }
            else 
            {
                ESP_LOGE(TAG, "Buffer overflow, received %d bytes", evt->data_len);
                return ESP_FAIL;
            }
        ////////////////////////////////////////////////////////////////////////
        }
        break;
    
    case HTTP_EVENT_ON_FINISH:// 接收数据结束事件
        if (output_buffer != NULL)
        {
            output_buffer[output_len] = '\0';   
            ESP_LOGI("http_event_handler_cb","buffer = %s", output_buffer);//原始json数据
        //////////////////////////////////////////////////////////////////
            //解析json数据
            parsed_json(output_buffer);

            printf("地区：%s\r\n", now_weather.location);
            printf("温度：%d\r\n", now_weather.temperature);
            printf("天气：%s\r\n", now_weather.weather_situation);
        //////////////////////////////////////////////////////////////////
            //释放output_buffer，重置相关变量
            output_len = 0; 
            free(output_buffer);
            output_buffer = NULL;
        }
        break;
    
    case HTTP_EVENT_DISCONNECTED:// 接收数据事件
        ESP_LOGI("http_event_handler_cb", "HTTP Disconnected");
        if (output_buffer != NULL) 
        {
            output_len = 0; 
            free(output_buffer);
            output_buffer = NULL;
        }
        break;

    default:
        break;
    }

    return ESP_OK;
}

static void parsed_json(const char *buffer)
{
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL)
    {
        ESP_LOGE("parsed_json", "JSON parse error: %s", cJSON_GetErrorPtr());
    } 
    else
    {
        //获取JSON中的"results"字段，并检查它是否为数组。
        cJSON *results = cJSON_GetObjectItem(root, "results");//对应 JSON 的 {"results": [...]}
        if(results && cJSON_IsArray(results))
        {
            cJSON *result = cJSON_GetArrayItem(results, 0);//从 results 数组中获取第一个元素（results[0]）。
            //检查 result 是否有效（非 NULL），确保数组不为空。
            if(result)
            {
            ///////////////////////////////////////////////////////////////////////////////    
                cJSON *location = cJSON_GetObjectItem(result, "location");
                if (!location) {
                    ESP_LOGE("parsed_json", "location字段缺失");
                }
                cJSON *name = cJSON_GetObjectItem(location, "name");
                if(name)
                {
                    strncpy(now_weather.location, name->valuestring, sizeof(now_weather.location));
                    now_weather.location[sizeof(now_weather.location)-1] = '\0';
                }

            ///////////////////////////////////////////////////////////////////////////////    
                //"now": {"text": "阴", "code": "9", "temperature": "26"},
                // 提取now对象
                cJSON *now = cJSON_GetObjectItem(result, "now");
                if (!now) {
                    ESP_LOGE("parsed_json", "now字段缺失");
                }

                // 提取具体字段
                cJSON *temperature = cJSON_GetObjectItem(now, "temperature");
                cJSON *text = cJSON_GetObjectItem(now, "text");

                 if (temperature && text) 
                 {
                    now_weather.temperature = atoi(temperature->valuestring);//字符串转换为整数
                    strncpy(now_weather.weather_situation, text->valuestring, sizeof(now_weather.weather_situation));
                    now_weather.weather_situation[sizeof(now_weather.weather_situation)-1] = '\0';
                }
            ///////////////////////////////////////////////////////////////////////////////    
            }
        }
    }
}

void Get_weather_task(void *pvParameters)
{
    while(1)
    {
        get_weather();
        vTaskDelay(60000/ portTICK_PERIOD_MS);
    }
}

void get_weather(void)
{
    ESP_LOGI("http_get_weather", "http_get_weather");
    char url[256];
    snprintf(url, sizeof(url), 
        "http://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c",
        API_KEY, LOCATION);

    esp_http_client_config_t cfg = {
        .url = url,
        .event_handler = http_event_handler_cb,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL, // 使用全局 CA 证书
        .skip_cert_common_name_check = false,
        .timeout_ms = 5000, // 5 秒超时
    };
    http_client_handle = esp_http_client_init(&cfg);

    //发送请求，执行 esp_http_client 的所有操作
    esp_err_t err = esp_http_client_perform(http_client_handle);
    if(err == ESP_OK)
    {
        int http_status_code= esp_http_client_get_status_code(http_client_handle);
        int64_t content_length = esp_http_client_get_content_length(http_client_handle);

        if (http_status_code == 200) {
            ESP_LOGI("http_get_weather", "Request successful");
        } else {
            ESP_LOGW("http_get_weather", "Request failed with status %d", http_status_code);
        }

        ESP_LOGI("http_get_weather", "http get Status = %d, content_length = %lld", http_status_code,content_length);
    }
    else
    {
        ESP_LOGE("http_get_weather", "http get request failed: %s", esp_err_to_name(err));
    }

    //关闭连接，释放客户端资源
    ESP_ERROR_CHECK(esp_http_client_cleanup(http_client_handle));
}