package com.Study.vibeclean.dto.service.manual;

import com.Study.vibeclean.domain.manual.ManualPower;
import com.Study.vibeclean.domain.manual.ManualSpeed;
import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.manual.request.ManualPowerRequest;
import com.Study.vibeclean.dto.manual.request.ManualSpeedRequest;
import com.Study.vibeclean.dto.manual.response.ManualPowerResponse;
import com.Study.vibeclean.dto.manual.response.ManualSpeedResponse;
import com.Study.vibeclean.dto.repository.manual.ManualPowerRepository;
import com.Study.vibeclean.dto.repository.manual.ManualSpeedRepository;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
//import com.Study.vibeclean.dto.service.mqtt.MqttService;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Optional;

@Service
@RequiredArgsConstructor
public class ManualControlService {
    private final ManualPowerRepository manualPowerRepository;
    private final ManualSpeedRepository manualSpeedRepository;
    private final StatusRepository statusRepository;
    //private final MqttService mqttService;
    private String power;
    private final SensorRepository sensorRepository;

    @PostConstruct
    public void init (){
        statusRepository.deleteAll();
        sensorRepository.deleteAll();// 꺼졌다고 판단하여 로그에 출력 및 db에 저장된 모든 값들을 삭제한다.
        manualPowerRepository.deleteAll();
        manualSpeedRepository.deleteAll();
    }

    /*@Value("${mqtt.power-topic}")
    private String powerTopic;

    @Value("${mqtt.speed-topic}")
    private String speedTopic;*/

    // 수동조작 speed 들어온 값 DB 저장하는 메서드
    @Transactional
    public void setSpeed(ManualSpeedRequest manualSpeed){
        Status latest = statusRepository.findTopByOrderByTimeDesc();
        if (latest == null || !"ON".equalsIgnoreCase(latest.getPower())) {
            return; // 오프라인 또는 OFF → 무시
        }
        manualSpeedRepository.save(new ManualSpeed(manualSpeed.getFanSpeed()));
        // 수정하고자 하는 값을 DB에 저장함.

        //String payload = "{ \"fanSpeed\": " + manualSpeed.getFanSpeed() + " }";
        //mqttService.publish(speedTopic, payload);
        // 해당 토픽으로 수동조작하고자 하는 스피드 값을 보내줌.

    }

    // 수동 조작으로 들어온 모드 변환 값 DB 저장
    @Transactional
    public void setPower(ManualPowerRequest manualPower){
        power=manualPower.getPower();
        power = power.toUpperCase();
        Status latest = statusRepository.findTopByOrderByTimeDesc();

        if ("ON".equals(power)) {
            if (latest != null && "ON".equalsIgnoreCase(latest.getPower())) return;
            // ON 요청 → STM32로 송신/ 근데 이제 이미 ON 상태라면 무시
            manualPowerRepository.save(new ManualPower(power));
            //mqttService.publish(powerTopic, "{ \"power\": \"ON\" }");
        } else if ("OFF".equals(power)) {
            if (latest == null) return;
            // 이미 꺼져있는데 off값이 들어온 경우라면 무시한다.
            manualPowerRepository.save(new ManualPower(power));
            //mqttService.publish(powerTopic, "{ \"power\": \"OFF\" }");
        }


    }

    @Transactional
    public ManualSpeedResponse getSpeed(){
        Optional<ManualSpeed> manualSpeed=manualSpeedRepository.findTopByOrderByIdDesc();

        if (manualSpeed.isEmpty()){
            return new ManualSpeedResponse(4); // 만약 수동 조작으로 입력된 값이 없다면 기본 값 리턴
        }

        int speed = manualSpeed.get().getSpeed(); // 수동 조작으로 들어온 값을 넣음
        manualSpeedRepository.deleteAll(); // 값을 넣은 후 db를 다시 초기화 시켜줘야 한다 .
        // 이렇게 해야 이제 STM측에서 지속적으로 polling할 때, 이미 한 명령을 다시 가져가는 거를 막을 수 있다.
        // 한 번에 하나의 명령 즉 상태를 getter로 전달하고 그 값은 삭제해야 하는 것이다.

        return new ManualSpeedResponse(speed);
    }

    @Transactional
    public ManualPowerResponse getPower(){
        Optional<ManualPower> manualPower=manualPowerRepository.findTopByOrderByIdDesc();

        if(manualPower.isEmpty()){
            return new ManualPowerResponse(null); // 만약 수동 조작으로 입력된 값이 없다면 기본 값 리턴
        }

        String power = manualPower.get().getPower();
        manualPowerRepository.deleteAll(); // 위와 같은 원리

        return new ManualPowerResponse(power);
    }
}
