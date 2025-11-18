package com.Study.vibeclean.dto.service.sensor;

import com.Study.vibeclean.domain.sensor.Sensor;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.sensor.request.SensorRequest;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Optional;

@Service
public class SensorService {
    private SensorRepository sensorRepository;

    public SensorService(SensorRepository sensorRepository) {
        this.sensorRepository = sensorRepository;
    }

    @Transactional
    public void saveSensorValue(SensorRequest request){
        //sensorRepository.save(new Sensor(request.getX(),request.getY(),request.getZ())); // 중간 끝나고 다시 짜면서 많이 까먹었네;; jpa통해서 save할 때는, 객체로 저장해야 한다. 그리고 그 객체를 domain에다가 설정하는 것이고
        // 현재 토픽으로 값 받으면 바로 저장되게 해놨으므로 이 코드는 굳이 필요 없음 그래도 그냥 주석정도 처리는 해두자.


    }

    @Transactional
    public Sensor getSensorValue(){
        Optional<Sensor> optionalSensor = sensorRepository.findTopByOrderByIdDesc();
        if (optionalSensor.isEmpty()) {
            //  DB가 비어있을 경우 내가 지정한 기본 Sensor 값 반환
            return new Sensor(0.0F, 0.0F, 0.0F);
        }
        return optionalSensor.get();

    }
}
