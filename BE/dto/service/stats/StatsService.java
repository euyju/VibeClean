package com.Study.vibeclean.dto.service.stats;

import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.repository.stats.StatsRepository;
import com.Study.vibeclean.dto.stats.Stats;
import com.Study.vibeclean.dto.stats.response.FloorDistribution;
import com.Study.vibeclean.dto.stats.response.StatsResponse;
import com.Study.vibeclean.dto.status.response.RobotStatusResponse;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Service
public class StatsService {
    private StatsRepository statsRepository;

    public StatsService(StatsRepository statsRepository) {
        this.statsRepository = statsRepository;
    }

    @Transactional
    public StatsResponse RobotStatus(){
        Status statusInitial = statsRepository.findTopByOrderByTimeAsc();
        Status statusFinal =statsRepository.findTopByOrderByTimeDesc();
        List<Stats> stats=statsRepository.findAllByCurrentFloor();

        if (stats.isEmpty()){ // 만약 비어있다면 즉 아무 값도 없다면 이렇게 기본 값 리턴
            return new StatsResponse(new FloorDistribution(new ArrayList<>()),LocalDateTime.now(), LocalDateTime.now());
        }

        return new StatsResponse(new FloorDistribution(stats),statusInitial.getTime(),statusFinal.getTime());
    }
}
