package com.Study.vibeclean.controller.sensor;

import com.Study.vibeclean.domain.sensor.Sensor;
import com.Study.vibeclean.dto.sensor.request.SensorRequest;
import com.Study.vibeclean.dto.sensor.response.SensorResponse;
import com.Study.vibeclean.dto.service.manual.ManualControlService;
import com.Study.vibeclean.dto.service.sensor.SensorService;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class SensorController {
    private SensorService sensorService;

    public SensorController(SensorService sensorService) {
        this.sensorService = sensorService;
    }

    /*@PostMapping("/api/robot/sensor")
    public void saveSensorValue(@RequestBody SensorRequest request){
        //sensorService.saveSensorValue(request); // 이거는 내가 전에 기본적인 동작 확인하려고 만든 거라서 지금 토픽 구조에서는 안 씀


    }*/

    @GetMapping("/api/robot/sensor")
    public Sensor getSensorValue(){
        Sensor response=sensorService.getSensorValue();
        return response;
    }

}
