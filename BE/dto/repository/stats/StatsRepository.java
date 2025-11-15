package com.Study.vibeclean.dto.repository.stats;

import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.stats.Stats;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.List;

public interface StatsRepository extends JpaRepository<Status,Long> {
    @Query("SELECT s.currentFloor FROM Status s")
    List<Stats> findAllByCurrentFloor();

    Status findTopByOrderByTimeDesc();
    Status findTopByOrderByTimeAsc();


}
