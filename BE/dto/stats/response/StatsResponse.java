package com.Study.vibeclean.dto.stats.response;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.time.Duration;
import java.time.LocalDateTime;

@JsonPropertyOrder({ "totalRuntimeSeconds", "floorDistribution" }) // total시간부터 출력위함
public class StatsResponse {
    private int totalRuntimeSeconds;
    private LocalDateTime initialTime;
    private LocalDateTime finalTime;
    private FloorDistribution floorDistribution;

    // 원래라면, 시간도 측정해서 이제 해당 값을 생성자 보내줘야 하는데 아직까지는 우리가 직접 그거를 주기적으로 값ㅇ르
    // 줄 수가 없기 때문에 임의의 값을 넣고 만듦. 나중에 추가 구현 필요


    public StatsResponse(FloorDistribution floorDistribution, LocalDateTime initialTime, LocalDateTime finalTime) {
        this.floorDistribution = floorDistribution;
        this.finalTime = finalTime;
        this.initialTime = initialTime;
        this.totalRuntimeSeconds = (int) Duration.between(initialTime, finalTime).getSeconds();
    }

    public int getTotalRuntimeSeconds() {
        return totalRuntimeSeconds;
    }

    public FloorDistribution getFloorDistribution() {
        return floorDistribution;
    }
}
