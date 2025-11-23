/*
 * mqtt_comm.c
 *
 * MQTT 통신 관련 함수 구현
 * WizFi360 모듈을 사용한 MQTT 통신 기능 제공
 */

#include "mqtt_comm.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

/* MQTT 제어 상태 변수 정의 */
int g_powerOn = -1;
int g_fanSpeed = -1;
int g_modeManual = -1;
char g_direction[8] = "NULL";

/* MQTT 수신 라인 버퍼 */
char mqtt_rx_buf[256];
int mqtt_rx_len = 0;

/* 외부 UART 핸들 참조 */
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* 내부 디버그 함수 프로토타입 */
static void Debug_PrintControlState(const char *topic, const char *json);

void Send_AT_Command(UART_HandleTypeDef *huart_wiz, UART_HandleTypeDef *huart_term, const char *command, uint8_t *response_buffer, uint16_t buffer_size, uint32_t timeout)
{
    // 1. 터미널(huart_term, PuTTY)로 전송할 명령 표시
    char tx_log[64];
    int len = snprintf(tx_log, sizeof(tx_log), "\r\n[CMD] Sending: %s\r\n", command);
    HAL_UART_Transmit(huart_term, (uint8_t *)tx_log, len, HAL_MAX_DELAY);

    // 2. WizFi360 (huart_wiz)에 명령 전송 (명령어 끝에 CR+LF(\r\n) 필수)
    char cmd_with_crlf[128];
    snprintf(cmd_with_crlf, sizeof(cmd_with_crlf), "%s\r\n", command);
    HAL_UART_Transmit(huart_wiz, (uint8_t *)cmd_with_crlf, strlen(cmd_with_crlf), 500);

    // 3. 응답 수신 준비
    memset(response_buffer, 0, buffer_size); // 버퍼 초기화
    uint16_t index = 0;
    uint32_t start_tick = HAL_GetTick();

    // 4. WizFi360으로부터 응답 수신 (간단한 타임아웃 폴링 방식)
    while ((HAL_GetTick() - start_tick) < timeout)
    {
        uint8_t rx_byte;
        // HAL_UART_Receive는 지정된 타임아웃 내에 1바이트를 수신합니다.
        if (HAL_UART_Receive(huart_wiz, &rx_byte, 1, 100) == HAL_OK)
        {
            if (index < buffer_size - 1)
            {
                response_buffer[index++] = rx_byte;
            }
            // 새로운 바이트를 수신할 때마다 타임아웃을 연장합니다.
            start_tick = HAL_GetTick();
        }
    }

    // 5. 수신된 응답을 터미널(huart_term, PuTTY)로 출력
    char *rx_header = "[RSP] Response Received:\r\n";
    HAL_UART_Transmit(huart_term, (uint8_t *)rx_header, strlen(rx_header), HAL_MAX_DELAY);

    MQTT_ProcessResponseBuffer(response_buffer, index);

    // 수신된 raw 데이터를 그대로 터미널로 출력
    HAL_UART_Transmit(huart_term, response_buffer, index, HAL_MAX_DELAY);

    char *rx_footer = "\r\n[END] ----------------------------------\r\n";
    HAL_UART_Transmit(huart_term, (uint8_t *)rx_footer, strlen(rx_footer), HAL_MAX_DELAY);
}

// WizFi360에 AT 문자열만 보내고, 응답은 읽지 않는 버전 publish와 subscribe 시, 버퍼를 공동으로 사용하여 충돌 일어나는 걸
// 방지하기 위해서 별도로 꼭 필요하다.
void WizFi_SendOnly(UART_HandleTypeDef *huart_wiz,
                    UART_HandleTypeDef *huart_term,
                    const char *command)
{
    // 디버깅용으로 어떤 명령 보냈는지 터미널에만 찍고
    char tx_log[64];
    int len = snprintf(tx_log, sizeof(tx_log),
                       "\r\n[CMD-TX] %s\r\n", command);
    HAL_UART_Transmit(huart_term, (uint8_t *)tx_log, len, HAL_MAX_DELAY);

    // WizFi360으로 명령 전송
    char cmd_with_crlf[128];
    snprintf(cmd_with_crlf, sizeof(cmd_with_crlf), "%s\r\n", command);
    HAL_UART_Transmit(huart_wiz,
                      (uint8_t *)cmd_with_crlf,
                      strlen(cmd_with_crlf),
                      500);
}

// 현재 control 상태를 PuTTY(USART2)에 출력하는 디버그용 함수
static void Debug_PrintControlState(const char *topic, const char *json)
{
    char buf[160];

    int len = snprintf(buf, sizeof(buf),
                       "\r\n[CONTROL] topic = %s\r\n"
                       "          json  = %s\r\n"
                       "          power = %d, speed = %d, mode = %d, dir = %s\r\n",
                       topic,
                       json,
                       g_powerOn,
                       g_fanSpeed,
                       g_modeManual,
                       g_direction);

    HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, HAL_MAX_DELAY);
}



/*
void Setup_WiFi_And_MQTT(void)
{
    // 1. Wi-Fi 연결
    Send_AT_Command(&huart3, &huart2, "AT+CWJAP=\"S24\",\"dial8787@@\"", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 8000);
    HAL_Delay(5000);

    // 2. TCP 연결 (로컬 Mosquitto 브로커)
    Send_AT_Command(&huart3, &huart2, "AT+CIPSTART=\"TCP\",\"192.168.178.61\",1883", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 5000);
    HAL_Delay(3000);

    // 3. MQTT 연결 설정 (User/Password/ClientID/KeepAlive)
    // 브로커 인증이 없으면 "" 사용
    Send_AT_Command(&huart3, &huart2, "AT+MQTTSET=\"\",\"\",\"STM32Client\",300", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 2000);
    HAL_Delay(1000);

    // 4. MQTT 토픽 설정
    // Publish: "vibeclean/robot1/telemetry", Subscribe: "vibeclean/robot1/control/#"
    Send_AT_Command(&huart3, &huart2,
        "AT+MQTTTOPIC=\"vibeclean/robot1/telemetry\",\"vibeclean/robot1/control/#\"",
        wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 2000);


    // 4-1. Publish QoS 설정 (예: QoS1)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTQOS=1", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
    HAL_Delay(500);

    // 5. MQTT CONNECT (Single connection, 인증 없음)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTCON=0,\"192.168.178.61\",1883", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 5000);
    HAL_Delay(3000);

    // 6. MQTT 구독 (토픽은 이미 AT+MQTTTOPIC로 설정)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTSUB", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 3000);
    HAL_Delay(1000);
}
*/


// -----------------------------------------------------
// MQTT 초기화용 공용 AT 명령 함수들
// -----------------------------------------------------

uint8_t MQTT_SetConfig(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *user,
    const char *pass,
    const char *clientID,
    int aliveTime,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTSET=\"%s\",\"%s\",\"%s\",%d",
             user, pass, clientID, aliveTime);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_SetTopics(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *pubTopic,
    const char *subTopic,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTTOPIC=\"%s\",\"%s\"",
             pubTopic, subTopic);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_SetQos(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    int qos,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+MQTTQOS=%d", qos);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 2000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_ConnectBroker(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *brokerIP,
    int port,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTCON=0,\"%s\",%d",
             brokerIP, port);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 6000);

    return strstr((char*)resp, "CONNECT") || strstr((char*)resp, "OK");
}

uint8_t MQTT_PublishJSON(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *jsonMsg,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTPUB=\"%s\"", jsonMsg);

    //Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    WizFi_SendOnly(huart_wiz, huart_term, cmd);
    //return strstr((char*)resp, "OK") != NULL;
    return 1;
}

uint8_t MQTT_Init_All(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term
){
    uint8_t resp[512];

    // -------- Wi-Fi 연결 --------
    Send_AT_Command(huart_wiz, huart_term, "AT", resp, sizeof(resp), 1000);
    Send_AT_Command(huart_wiz, huart_term, "AT+RST", resp, sizeof(resp), 5000);
    Send_AT_Command(huart_wiz, huart_term, "AT+CWMODE=1", resp, sizeof(resp), 2000);

    // WiFi 접속
    Send_AT_Command(huart_wiz, huart_term,
        "AT+CWJAP=\"S24\",\"dial8787@@\"",
        resp, sizeof(resp), 15000);

    // -------- MQTT 설정 --------
    if (!MQTT_SetConfig(huart_wiz, huart_term,
                        "", "", "vibeclean01", 300,
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetTopics(huart_wiz, huart_term,
                        "vibeclean/robot1/telemetry",
                        "vibeclean/robot1/control/#",
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetQos(huart_wiz, huart_term, 0, resp, sizeof(resp)))
        return 0;

    if (!MQTT_ConnectBroker(huart_wiz, huart_term,
                            "192.168.178.61", 1883,
                            resp, sizeof(resp)))
        return 0;

    return 1;
}


void Publish_Message(void)
{
    char cmd[256];

    const char *json_message =
        "{\"currentFloor\":\"Carpet\",\"fanSpeed\":3,"
        "\"position\":{\"x\":10,\"y\":5},"
        "\"sensor\":{\"x\":0.1,\"y\":0.2,\"z\":0.3}}";

    // topic은 이미 AT+MQTTTOPIC로 설정되어 있으므로 메시지만 전달
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=\"%s\"", json_message);
    Send_AT_Command(&huart3, &huart2, cmd, wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
//    WizFi_SendOnly(&huart3, &huart2, cmd); //wizfi통해서 보내기만 하는 애
}

// 토픽과 JSON을 보고 상태 변수에 반영
void HandleControlJson(const char *topic, const char *json)
{
    if (strstr(topic, "control/power")) {
        if (strstr(json, "\"ON\""))  g_powerOn = 1;
        if (strstr(json, "\"OFF\"")) g_powerOn = 0;
    }
    else if (strstr(topic, "control/speed")) {
        int v = 0;
        // 숫자 하나만 뽑는 단순 파서 (예: {"fanSpeed":2})
        sscanf(json, "%*[^0-9]%d", &v);
        if (v < 0) v = 0;
        if (v > 3) v = 3;
        g_fanSpeed = v;
    }
    else if (strstr(topic, "control/mode")) {
        if (strstr(json, "\"MANUAL\"")) g_modeManual = 1;
        if (strstr(json, "\"AUTO\""))   g_modeManual = 0;
    }
    else if (strstr(topic, "control/direction")) {
        if (strstr(json, "\"FWD\""))   strcpy(g_direction, "FWD");
        else if (strstr(json, "\"BACK\""))  strcpy(g_direction, "BACK");
        else if (strstr(json, "\"LEFT\""))  strcpy(g_direction, "LEFT");
        else if (strstr(json, "\"RIGHT\"")) strcpy(g_direction, "RIGHT");
        else if (strstr(json, "\"STOP\""))  strcpy(g_direction, "STOP");
    }

    Debug_PrintControlState(topic, json);
}

// WizFi360이 보낸 한 줄(line)을 토픽 / json 으로 분리
void ParseMqttLine(char *line, int len)
{
    line[len] = '\0';

    // "topic->\"...\"" 형식에서 화살표 위치 찾기
    char *arrow = strstr(line, "->");
    if (!arrow) return;

    *arrow = '\0';
    char *topic = line;
    char *payload = arrow + 2;

    // 공백/따옴표 건너뛰기
    while (*payload == ' ' || *payload == '"' || *payload == '\r' || *payload == '\n')
        payload++;

    // JSON 시작 '{' 위치 찾기
    char *brace = strchr(payload, '{');
    if (!brace) return;

    char *end = strrchr(brace, '}');
    if (!end) return;
    *(end + 1) = '\0';   // JSON 문자열을 '\0'로 끝나게

    HandleControlJson(topic, brace);
}

// WizFi360으로부터 들어오는 subscribe 메시지 처리
void MQTT_ProcessIncoming(void)
{
    uint8_t ch;

    // timeout=0 으로 non-blocking 폴링
    while (HAL_UART_Receive(&huart3, &ch, 1, 0) == HAL_OK) {
        if (mqtt_rx_len < (int)sizeof(mqtt_rx_buf) - 1) {
            mqtt_rx_buf[mqtt_rx_len++] = (char)ch;
            mqtt_rx_buf[mqtt_rx_len] = '\0';
        }

        // 한 줄 끝(일반적으로 \n 기준)이면 파싱
        if (ch == '\n') {
            ParseMqttLine(mqtt_rx_buf, mqtt_rx_len);
            mqtt_rx_len = 0;
            mqtt_rx_buf[0] = '\0';
        }
    }
}

// WizFi360 응답 버퍼 전체에서 "topic -> { json }" 형태의 구독 메시지를 찾아 처리
void MQTT_ProcessResponseBuffer(uint8_t *buf, uint16_t len)
{
    char line[256];
    uint16_t pos = 0;

    for (uint16_t i = 0; i < len; i++) {
        char c = (char)buf[i];

        // CR은 무시
        if (c == '\r') {
            continue;
        }

        // 줄 끝이거나 line 버퍼가 꽉 찬 경우
        if (c == '\n' || pos >= sizeof(line) - 1) {

            if (pos > 0) {
                line[pos] = '\0';

                // "->" 들어간 라인만 MQTT 메시지로 인정
                if (strstr(line, "->")) {
                    ParseMqttLine(line, strlen(line));
                }
            }

            pos = 0;
        }
        else {
            // 일반 문자는 line에 저장
            line[pos++] = c;
        }
    }

    // ★ 마지막 라인이 '\n' 없이 종료되었을 때 처리
    if (pos > 0) {
        line[pos] = '\0';
        if (strstr(line, "->")) {
            ParseMqttLine(line, strlen(line));
        }
    }
}


/*void ApplyManualControl(void)
{
    if (!g_powerOn) {
        // 전원 OFF면 무조건 정지
        stop_all_motors();
        return;
    }

    if (!g_modeManual) {
        // AUTO 모드에서는 수동 방향 명령 무시
        return;
    }

    // fanSpeed(0~3)를 PWM으로 단순 매핑 (원하는 대로 조정 가능)
    uint16_t pwm = 0;
    switch (g_fanSpeed) {
        case 0: pwm = 0;                break;
        case 1: pwm = BASE_SPEED / 2;   break;
        case 2: pwm = BASE_SPEED;       break;
