/*
 * ESP8266_HAL.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllerstech
 */

#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_


// Wi-Fi 초기화 함수
void ESP_Init(char *SSID, char *PASSWD);

// HTTP GET 요청
int ESP_HTTP_Get(char *host, char *path);

// HTTP POST 요청
int ESP_HTTP_Post(char *host, char *path, char *json);

int ESP_HTTP_Get_Value(char *host, char *path, char *key, char *value_out);


#endif /* INC_ESP8266_HAL_H_ */
