package com.Study.vibeclean.dto.repository.manual;

import com.Study.vibeclean.domain.manual.ManualDirection;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface ManualDirectionRepository extends JpaRepository<ManualDirection,Integer> {
    Optional<ManualDirection> findTopByOrderByIdDesc();
}
