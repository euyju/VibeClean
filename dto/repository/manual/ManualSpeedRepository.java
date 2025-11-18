package com.Study.vibeclean.dto.repository.manual;

import com.Study.vibeclean.domain.manual.ManualSpeed;
import com.Study.vibeclean.domain.sensor.Sensor;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface ManualSpeedRepository extends JpaRepository<ManualSpeed,Integer> {
    Optional<ManualSpeed> findTopByOrderByIdDesc();
    
}
