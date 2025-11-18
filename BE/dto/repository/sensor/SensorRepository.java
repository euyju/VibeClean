package com.Study.vibeclean.dto.repository.sensor;

import com.Study.vibeclean.domain.sensor.Sensor;
import com.Study.vibeclean.domain.status.Status;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.Optional;

public interface SensorRepository extends JpaRepository<Sensor, Integer> {
    Optional<Sensor> findTopByOrderByIdDesc();

}
