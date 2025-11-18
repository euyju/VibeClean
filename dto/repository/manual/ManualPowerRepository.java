package com.Study.vibeclean.dto.repository.manual;

import com.Study.vibeclean.domain.manual.ManualPower;
import com.Study.vibeclean.domain.manual.ManualSpeed;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface ManualPowerRepository extends JpaRepository<ManualPower, Integer> {
    Optional<ManualPower> findTopByOrderByIdDesc();

}
