package com.Study.vibeclean.dto.service.mqtt;

import com.Study.vibeclean.dto.service.mqtt.TelemetryHandler;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.nio.charset.StandardCharsets;

@Slf4j // LomBok 사용을 위한 어노테이션 이거를 이용해서 log 활용 가능
@Service
@RequiredArgsConstructor // 이거는 final로 생성되는 변수들 자동으로 생성자 만들어줌 그에 따라 의존성 주입도 바로 됨.
// 내가 기존에 사용하던 방법인 변수 선언 + 생성자보다 더 간단한 방법
public class MqttService {
    private final MqttClient mqttClient;
    private final TelemetryHandler telemetryHandler;

    @Value("${mqtt.telemetry-topic}")
    private String telemetryTopic;

    @PostConstruct // 이 어노테이션은 완전히 초기화 된 후 한 번 무조건 실행하는 것을 의미한다.
    public void init() throws MqttException {
        // STM32 → 서버: telemetry 구독
        mqttClient.subscribe(telemetryTopic, (topic, message) -> { // 토픽을 구독하고, 받은 값을 payload에 넣고 UTF 형식 바꿈
            String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
            log.debug("Received MQTT: topic={}, payload={}", topic, payload); // 어떤 토픽에서 구독받은 건지 그리고 내용 출력
            telemetryHandler.handleTelemetry(payload); // 받은 내용을 핸들러로 넘겨서 DB에 저장한다.
        });
    }

    public void publish(String topic, String payload) {
        try {
            MqttMessage message = new MqttMessage(payload.getBytes(StandardCharsets.UTF_8));
            message.setQos(1);
            mqttClient.publish(topic, message);
        } catch (MqttException e) {
            log.error("Failed to publish MQTT message", e);
        }
    }
}
