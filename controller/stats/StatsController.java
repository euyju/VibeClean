package com.Study.vibeclean.controller.stats;

import com.Study.vibeclean.dto.service.stats.StatsService;
import com.Study.vibeclean.dto.stats.response.StatsResponse;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class StatsController {
    private StatsService statsService;

    public StatsController(StatsService statsService) {
        this.statsService = statsService;
    }

    @GetMapping("/api/robot/stats")
    public StatsResponse RobotStats(){
        StatsResponse statsResponse=statsService.RobotStatus();
        return statsResponse;
    }
}
