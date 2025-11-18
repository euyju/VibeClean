/*
package com.Study.vibeclean.dto.service.stm;

import com.Study.vibeclean.domain.manual.ManualMode;
import com.Study.vibeclean.domain.sensor.Sensor;
import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.repository.manual.ManualModeRepository;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
import com.Study.vibeclean.dto.stm.request.StmRequest;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Service
@RequiredArgsConstructor
public class StmService {
    private final SensorRepository sensorRepository;
    private final StatusRepository statusRepository;
    private final ManualModeRepository manualModeRepository;

    public void saveValue(StmRequest request){
        ManualMode manualMode = manualModeRepository.findTopByOrderByIdDesc();

        statusRepository.save(new Status("ON",request.getCurrentFloor(),request.getFanSpeed(),
                request.getPosition().getX(),request.getPosition().getY() ,LocalDateTime.now(),manualMode.getMode()));

        sensorRepository.save(new Sensor(request.getSensor().getX(),request.getSensor().getY(),
                request.getSensor().getZ()));
    }

}
*/
