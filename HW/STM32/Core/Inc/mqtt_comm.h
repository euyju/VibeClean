/*
 * mqtt_comm.h
 *
 * MQTT 통신 관련 함수 헤더 파일
 * WizFi360 모듈을 사용한 MQTT 통신 기능 제공
 */

#ifndef INC_MQTT_COMM_H_
#define INC_MQTT_COMM_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* MQTT 제어 상태 변수 (외부에서 접근 가능) */
extern int g_powerOn;           // 0 = OFF, 1 = ON
extern int g_fanSpeed;          // 0~3
extern int g_modeManual;        // 0 = AUTO, 1 = MANUAL
extern char g_direction[8];     // "FWD","BACK","LEFT","RIGHT","STOP"

/* MQTT 수신 버퍼 */
extern char mqtt_rx_buf[256];
extern int mqtt_rx_len;

/* AT 명령 전송 및 응답 수신 함수 */
void Send_AT_Command(UART_HandleTypeDef *huart_wiz,
                     UART_HandleTypeDef *huart_term,
                     const char *command,
                     uint8_t *response_buffer,
                     uint16_t buffer_size,
                     uint32_t timeout);

/* WizFi360에 명령만 전송 (응답 수신 없음) */
void WizFi_SendOnly(UART_HandleTypeDef *huart_wiz,
                    UART_HandleTypeDef *huart_term,
                    const char *command);

/* MQTT 설정 함수들 */
uint8_t MQTT_SetConfig(UART_HandleTypeDef *huart_wiz,
                       UART_HandleTypeDef *huart_term,
                       const char *user,
                       const char *pass,
                       const char *clientID,
                       int aliveTime,
                       uint8_t *resp, uint16_t resp_size);

uint8_t MQTT_SetTopics(UART_HandleTypeDef *huart_wiz,
                       UART_HandleTypeDef *huart_term,
                       const char *pubTopic,
                       const char *subTopic,
                       uint8_t *resp, uint16_t resp_size);

uint8_t MQTT_SetQos(UART_HandleTypeDef *huart_wiz,
                    UART_HandleTypeDef *huart_term,
                    int qos,
                    uint8_t *resp, uint16_t resp_size);

uint8_t MQTT_ConnectBroker(UART_HandleTypeDef *huart_wiz,
                           UART_HandleTypeDef *huart_term,
                           const char *brokerIP,
                           int port,
                           uint8_t *resp, uint16_t resp_size);

uint8_t MQTT_PublishJSON(UART_HandleTypeDef *huart_wiz,
                         UART_HandleTypeDef *huart_term,
                         const char *jsonMsg,
                         uint8_t *resp, uint16_t resp_size);

/* MQTT 초기화 (WiFi 연결 + MQTT 설정) */
uint8_t MQTT_Init_All(UART_HandleTypeDef *huart_wiz,
                      UART_HandleTypeDef *huart_term);

/* MQTT 메시지 처리 함수들 */
void MQTT_ProcessIncoming(void);
void MQTT_ProcessResponseBuffer(uint8_t *buf, uint16_t len);
void ParseMqttLine(char *line, int len);
void HandleControlJson(const char *topic, const char *json);

/* MQTT 메시지 전송 */
void Publish_Message(void);

#endif /* INC_MQTT_COMM_H_ */
