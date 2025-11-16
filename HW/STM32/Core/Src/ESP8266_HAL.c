//ESP8266_HAL.c

#include "UartRingbuffer_multi.h"
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

#define wifi_uart &huart3
#define pc_uart   &huart2

char buffer[512];

void ESP_Init(char *SSID, char *PASSWD)
{
    char data[128];
    Ringbuf_init();

    Uart_sendstring("AT+RST\r\n", wifi_uart);
    Uart_sendstring("RESETTING...\r\n", pc_uart);
    HAL_Delay(3000);

    // Test AT
    Uart_sendstring("AT\r\n", wifi_uart);
    if (Wait_for("OK", wifi_uart))
        Uart_sendstring("AT OK\r\n", pc_uart);
    else
        Uart_sendstring("AT FAIL\r\n", pc_uart);

    // Station mode
    Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
    Wait_for("OK", wifi_uart);
    Uart_sendstring("CWMODE=1 OK\r\n", pc_uart);

    // Connect to AP
    sprintf(data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
    Uart_sendstring(data, wifi_uart);
    Uart_sendstring("Connecting to Wi-Fi...\r\n", pc_uart);

    if (Wait_for("WIFI GOT IP", wifi_uart))
        Uart_sendstring("Connected to Wi-Fi\r\n", pc_uart);
    else
        Uart_sendstring("Wi-Fi Connect Failed\r\n", pc_uart);

    // Get IP
    Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
    Wait_for("STAIP,\"", wifi_uart);
    Copy_upto("\"", buffer, wifi_uart);
    sprintf(data, "IP Address: %s\r\n", buffer);
    Uart_sendstring(data, pc_uart);
}

/* HTTP GET 요청 */
int ESP_HTTP_Get(char *host, char *path)
{
    char cmd[256];
    char rx[1024] = {0};

    // Connect TCP
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",8080\r\n", host);
    Uart_sendstring(cmd, wifi_uart);

    if (!Wait_for("CONNECT", wifi_uart)) {
        Uart_sendstring("TCP Connect Failed\r\n", pc_uart);
        return 0;
    }
    Uart_sendstring("TCP Connected\r\n", pc_uart);

    // Build GET request
    sprintf(cmd,
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: STM32-ESP8266\r\n"
            "Connection: close\r\n\r\n",
            path, host);

    int len = strlen(cmd);

    // Send length
    char sendcmd[32];
    sprintf(sendcmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(sendcmd, wifi_uart);

    if (!Wait_for(">", wifi_uart)) {
        Uart_sendstring("CIPSEND Fail\r\n", pc_uart);
        return 0;
    }

    Uart_sendstring(cmd, wifi_uart);
    Uart_sendstring("HTTP GET Sent\r\n", pc_uart);

    // Wait and read response
    Copy_upto("CLOSED", rx, wifi_uart);
    Uart_sendstring("Response:\r\n", pc_uart);
    Uart_sendstring(rx, pc_uart);

    // Close connection
    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    Wait_for("OK", wifi_uart);

    Uart_sendstring("\r\n--- END ---\r\n", pc_uart);
    return 1;
}


int ESP_HTTP_Get_Value(char *host, char *path, char *key, char *value_out)
{
    char cmd[256];
    char rx[1024] = {0};
    char *start, *end;

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",8080\r\n", host);
    Uart_sendstring(cmd, wifi_uart);

    if (!Wait_for("CONNECT", wifi_uart)) {
        Uart_sendstring("TCP Connect Failed\r\n", pc_uart);
        return 0;
    }

    sprintf(cmd,
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: STM32-ESP8266\r\n"
            "Connection: close\r\n\r\n",
            path, host);

    int len = strlen(cmd);
    char sendcmd[32];
    sprintf(sendcmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(sendcmd, wifi_uart);

    if (!Wait_for(">", wifi_uart)) return 0;

    Uart_sendstring(cmd, wifi_uart);
    Copy_upto("CLOSED", rx, wifi_uart);

    // JSON 부분 추출
    start = strchr(rx, '{');
    end   = strrchr(rx, '}');
    if (start && end && (end > start)) {
        *end = '\0';
        char json[256];
        strcpy(json, start);

        // 간단 파싱: "key": value 형식 찾기
        char *keypos = strstr(json, key);
        if (keypos) {
            keypos += strlen(key) + 2; // "key":
            while (*keypos == ' ' || *keypos == '\"') keypos++; // 공백/따옴표 제거

            int i = 0;
            while (*keypos && *keypos != '\"' && *keypos != ',' && *keypos != '}') {
                value_out[i++] = *keypos++;
            }
            value_out[i] = '\0';
        } else {
            strcpy(value_out, "N/A");
        }

        Uart_sendstring("Parsed value: ", pc_uart);
        Uart_sendstring(value_out, pc_uart);
        Uart_sendstring("\r\n", pc_uart);
    }

    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    Wait_for("OK", wifi_uart);
    return 1;
}


/* HTTP POST 요청 */
int ESP_HTTP_Post(char *host, char *path, char *json)
{
    char cmd[512];
    char rx[1024] = {0};

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", host);
    Uart_sendstring(cmd, wifi_uart);

    if (!Wait_for("CONNECT", wifi_uart)) {
        Uart_sendstring("TCP Connect Failed\r\n", pc_uart);
        return 0;
    }
    Uart_sendstring("TCP Connected\r\n", pc_uart);

    // Build POST request
    sprintf(cmd,
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "User-Agent: STM32-ESP8266\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            path, host, (int)strlen(json), json);

    int len = strlen(cmd);

    char sendcmd[32];
    sprintf(sendcmd, "AT+CIPSEND=%d\r\n", len);
    Uart_sendstring(sendcmd, wifi_uart);

    if (!Wait_for(">", wifi_uart)) {
        Uart_sendstring("CIPSEND Fail\r\n", pc_uart);
        return 0;
    }

    Uart_sendstring(cmd, wifi_uart);
    Uart_sendstring("HTTP POST Sent\r\n", pc_uart);

    Copy_upto("CLOSED", rx, wifi_uart);
    Uart_sendstring("Response:\r\n", pc_uart);
    Uart_sendstring(rx, pc_uart);

    // Close connection
    Uart_sendstring("AT+CIPCLOSE\r\n", wifi_uart);
    Wait_for("OK", wifi_uart);

    Uart_sendstring("\r\n--- END ---\r\n", pc_uart);
    return 1;
}
