package com.Study.vibeclean.config;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
@RequiredArgsConstructor
@Getter
public class MqttConfig {

    @Value("${mqtt.broker-url}") //이 어노테이션으로 우리가 yml에 설정한 환경 변수 값 아래 변수에 주입
    private String brokerUrl;

    @Value("${mqtt.client-id}")
    private String clientId;

    @Bean
    public MqttClient mqttClient() throws MqttException {
        MqttClient client = new MqttClient(brokerUrl, clientId, new MemoryPersistence()); // MQTT client 서버 객체 생성
        MqttConnectOptions options = new MqttConnectOptions(); // MQTT 통신 시 연결 설정 객체 생성
        options.setAutomaticReconnect(true);// 연결 끊어지면 자동으로 재연결 허용
        options.setCleanSession(true);// 세션 끝났다가 재 시작 시 기존 데이터 초기화 후 시작

        // MQTT 와 연결 시도
        System.out.println("[MQTT] Connecting to broker: " + brokerUrl + " with clientId=" + clientId);
        client.connect(options); // 이 코드를 통해서 실제로 mosquittto에게 client자격으로 연결 시도
        // 연결 성공 시
        System.out.println("[MQTT] Connected: " + client.isConnected());

        return client;
    }
}
