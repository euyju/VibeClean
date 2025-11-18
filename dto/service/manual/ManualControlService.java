package com.Study.vibeclean.dto.service.manual;

import com.Study.vibeclean.domain.manual.ManualDirection;
import com.Study.vibeclean.domain.manual.ManualMode;
import com.Study.vibeclean.domain.manual.ManualPower;
import com.Study.vibeclean.domain.manual.ManualSpeed;
import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.manual.request.ManualDirectionRequest;
import com.Study.vibeclean.dto.manual.request.ManualModeRequest;
import com.Study.vibeclean.dto.manual.request.ManualPowerRequest;
import com.Study.vibeclean.dto.manual.request.ManualSpeedRequest;
import com.Study.vibeclean.dto.manual.response.ManualDirectionResponse;
import com.Study.vibeclean.dto.manual.response.ManualModeResponse;
import com.Study.vibeclean.dto.manual.response.ManualPowerResponse;
import com.Study.vibeclean.dto.manual.response.ManualSpeedResponse;
import com.Study.vibeclean.dto.repository.manual.ManualDirectionRepository;
import com.Study.vibeclean.dto.repository.manual.ManualModeRepository;
import com.Study.vibeclean.dto.repository.manual.ManualPowerRepository;
import com.Study.vibeclean.dto.repository.manual.ManualSpeedRepository;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
//import com.Study.vibeclean.dto.service.mqtt.MqttService;
import com.Study.vibeclean.dto.service.mqtt.MqttService;
import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Objects;
import java.util.Optional;

@Service
@RequiredArgsConstructor
public class ManualControlService {
    private final ManualPowerRepository manualPowerRepository;
    private final ManualSpeedRepository manualSpeedRepository;
    private final StatusRepository statusRepository;
    private final MqttService mqttService;
    private String power;
    private final SensorRepository sensorRepository;
    private final ManualModeRepository manualModeRepository;
    private final ManualDirectionRepository manualDirectionRepository;

    @PostConstruct
    public void init (){
        statusRepository.deleteAll();
        sensorRepository.deleteAll();//맨 처음에 서버가 초기화된다면, DB 역시 기본 init상태로 만든다.
        manualPowerRepository.deleteAll();
        manualSpeedRepository.deleteAll();
        manualDirectionRepository.deleteAll();
        manualModeRepository.deleteAll();
        manualModeRepository.save(new ManualMode("AUTO")); // 수동 조작을 할 때, mode를 다루는 경우, 기본 초기화 값 자체가 auto가 들어간 상태이므로,
        // 모든 행을 다 삭제한 후에, 기본 값인 AUTO를 넣어둔다.
    }

    @Value("${mqtt.power-topic}")
    private String powerTopic;

    @Value("${mqtt.speed-topic}")
    private String speedTopic;

    @Value("${mqtt.mode-topic}")
    private String modeTopic;

    @Value("${mqtt.direction-topic}")
    private String directionTopic;

    // 수동조작 speed 들어온 값 DB 저장하는 메서드
    @Transactional
    public void setSpeed(ManualSpeedRequest manualSpeed){
        Status latest = statusRepository.findTopByOrderByTimeDesc();
        if (latest == null || !"ON".equalsIgnoreCase(latest.getPower())) {
            return; // 오프라인 또는 OFF → 무시
        }

        //http용
        //manualSpeedRepository.save(new ManualSpeed(manualSpeed.getFanSpeed()));
        // 수정하고자 하는 값을 DB에 저장함.

        String payload = "{ \"fanSpeed\": " + manualSpeed.getFanSpeed() + " }";
        mqttService.publish(speedTopic, payload);
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
            // ON 요청 → STM32로 전송 위한 DB저 장/ 근데 이제 이미 ON 상태라면 무시
            //manualPowerRepository.save(new ManualPower(power)); http용
            mqttService.publish(powerTopic, "{ \"power\": \"ON\" }");
        } else if ("OFF".equals(power)) {
            if (latest == null) return;
            // 이미 꺼져있는데 off값이 들어온 경우라면 무시한다.
            //manualPowerRepository.save(new ManualPower(power)); http용
            mqttService.publish(powerTopic, "{ \"power\": \"OFF\" }");
        }


    }

    @Transactional
    public void setMode(ManualModeRequest request){
        Status latestStatus = statusRepository.findTopByOrderByTimeDesc();
        ManualMode latestMode= manualModeRepository.findTopByOrderByIdDesc();

        if(latestStatus==null || Objects.equals(latestMode.getMode(), request.getMode())){
            return ;
        }//STM 전원 자체가 꺼져있거나, 아니면 이미 바꾸고자 FE에서 넘어온 모드 값이 이미 해당 상태면 무시한다

        manualModeRepository.save(new ManualMode(request.getMode()));
        //STM 전원이 켜져있고, mode가 현재 상태에서 다른 상태로 바꾸고자 하는 거면 DB에 저장한다.

        mqttService.publish(modeTopic, "{ \"mode\": \""+request.getMode()+ "\" }");
        //MQTT로 값을 전달해준다.
    }

    @Transactional
    public void setDirection(ManualDirectionRequest request){
        Status latestStatus=statusRepository.findTopByOrderByTimeDesc();
        ManualMode latestMode = manualModeRepository.findTopByOrderByIdDesc();

        if(latestStatus==null || Objects.equals(latestMode.getMode(), "AUTO")){
            return;
        } // 만약 FE에서 방향키를 눌렀는데 STM이 꺼져있는 상태이거나 아니면, 켜져는 있지만 자동 모드로 돌아가고 있다면 무시

        if (Objects.equals(request.getDirection(), "STOP")){ // 받은 direction이 STOP이라면, 사용자가 방향키를 누르다가 뗀 것을 의미함
            manualDirectionRepository.deleteAll();  // 사용자가 방향키를 뗀다면, 해당 table을 모두 삭제해서 초기로 만들어준다.
            return ;
        }

        manualDirectionRepository.save(new ManualDirection(request.getDirection()));
        //STM이 켜져있는 상태이고, 또 manual모드인 경우에만, 또 사용자가 방향키를 누른 경우에(not 손 뗌) DB에 저장됨

        // 원래 이 코드랑 위에 STOP 코드는 http 용이라서 //치는 게 맞긴 한데...ㅣㅣ흠....걍...걍...냅둬

        mqttService.publish(directionTopic, "{ \"direction\": \""+request.getDirection()+ "\" }");


    }

/*    @Transactional
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

    @Transactional
    public ManualModeResponse getMode(){
        ManualMode latestMode = manualModeRepository.findTopByOrderByIdDesc();
        return new ManualModeResponse(latestMode.getMode());
    }

    @Transactional
    public ManualDirectionResponse getDirection(){
        Optional<ManualDirection> manualDirection = manualDirectionRepository.findTopByOrderByIdDesc();

        // 만약 direction TABLE에 아무런 값도 들어있지 않다면, null값을 리턴한다.
        //좀 더 구체적으로 적어보자면, null값이 리턴되는 경우는, 1. Mode가 AUTO인 경우 2. STM 전원 자체가 꺼져있는 경우
        //3. Mode는 Manual이지만, FE측에서 방향키를 입력하지 않은 경우/ 이렇게 3가지 경우가 된다.
        return manualDirection.map(direction -> new ManualDirectionResponse(direction.getDirection()))
                .orElseGet(() -> new ManualDirectionResponse(null));
        // Mode가 Manual이고, 방향키를 누르고 있는 경우 해당 방향키의 값이 전달되게 된다.
    }*/
}
