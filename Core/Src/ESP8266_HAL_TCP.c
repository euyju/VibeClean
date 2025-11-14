// ESP8266_HAL_TCP.c
#include "ESP8266_HAL_TCP.h"
#include "UartRingbuffer_multi.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

#define wifi_uart &huart3
#define pc_uart   &huart2

// TCP 연결 초기화 (한 번만)
int ESP_TCP_Init(char *host, uint16_t port)
{
    char cmd[128];

    // 다중 연결 모드 활성화
    Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
    if (!Wait_for("OK", wifi_uart)) {
        Uart_sendstring("CIPMUX Fail\r\n", pc_uart);
        return 0;
    }

    // TCP 연결 (채널 0)
    sprintf(cmd, "AT+CIPSTART=0,\"TCP\",\"%s\",%d\r\n", host, port);
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for("CONNECT", wifi_uart)) {
        Uart_sendstring("TCP Connect Failed\r\n", pc_uart);
        return 0;
    }

    Uart_sendstring("TCP Connected (channel 0)\r\n", pc_uart);
    return 1;
}

// TCP 데이터 전송
int ESP_TCP_Send(int channel, char *data)
{
    char cmd[32];
    int len = strlen(data);

    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", channel, len); // 채널, 길이
    Uart_sendstring(cmd, wifi_uart);
    if (!Wait_for(">", wifi_uart)) {
        Uart_sendstring("CIPSEND Fail\r\n", pc_uart);
        return 0;
    }

    Uart_sendstring(data, wifi_uart);
    return 1;
}

// TCP 응답 수신
int ESP_TCP_Receive(int channel, char *buffer, int max_len)
{
    memset(buffer, 0, max_len);
    Copy_upto("CLOSED", buffer, wifi_uart); // 간단히 CLOSED까지 읽기
    return strlen(buffer);
}

// TCP 연결 종료
int ESP_TCP_Close(int channel)
{
    char cmd[32];
    sprintf(cmd, "AT+CIPCLOSE=%d\r\n", channel);
    Uart_sendstring(cmd, wifi_uart);
    Wait_for("OK", wifi_uart);
    return 1;
}
