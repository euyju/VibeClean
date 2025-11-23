// ESP8266_HAL_TCP.h
#ifndef ESP8266_HAL_TCP_H
#define ESP8266_HAL_TCP_H

#include "stdint.h"

int ESP_TCP_Init(char *host, uint16_t port);
int ESP_TCP_Send(int channel, char *data);
int ESP_TCP_Receive(int channel, char *buffer, int max_len);
int ESP_TCP_Close(int channel);

#endif
