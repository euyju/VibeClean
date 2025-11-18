package com.Study.vibeclean.dto.repository.manual;

import com.Study.vibeclean.domain.manual.ManualMode;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface ManualModeRepository extends JpaRepository<ManualMode, Integer> {
    ManualMode findTopByOrderByIdDesc();
}
