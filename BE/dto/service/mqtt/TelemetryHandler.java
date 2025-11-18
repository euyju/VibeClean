package com.Study.vibeclean.dto.service.mqtt;

import com.Study.vibeclean.dto.repository.manual.ManualModeRepository;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.Study.vibeclean.domain.sensor.Sensor;
import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.mqtt.TelemetryMessage;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Slf4j
@Service
@RequiredArgsConstructor
public class TelemetryHandler {
    // ObjectMapper는 Json문자열을 java객체로 변환해주는 Jackson라이브러리의 핵심 클래스이다.
    private final ObjectMapper objectMapper = new ObjectMapper();
    private final StatusRepository statusRepository;
    private final SensorRepository sensorRepository;
    private final ManualModeRepository manualModeRepository;

    public void handleTelemetry(String payload) {
        try {// 이 바로 아래 코드로 json문자열을 java 객체로 만들어준다. 매우 편리 ㅎㅎ
            TelemetryMessage msg = objectMapper.readValue(payload, TelemetryMessage.class);

            // 1) status 저장
            Status status = new Status("ON", msg.getCurrentFloor(),msg.getFanSpeed(),
                    msg.getPosition().getX(),msg.getPosition().getY(),LocalDateTime.now(),
                    manualModeRepository.findTopByOrderByIdDesc().getMode());
            statusRepository.save(status);

            // 2) sensor 저장
            TelemetryMessage.SensorData sd = msg.getSensor();
            if (sd != null) {
                Sensor sensor = new Sensor(sd.getX(),sd.getY(),sd.getZ());
                sensorRepository.save(sensor);
            }

        } catch (Exception e) {
            log.error("Failed to handle telemetry payload: {}", payload, e);
        }
    }
}
