package com.Study.vibeclean.dto.stats.response;

import com.Study.vibeclean.dto.stats.Stats;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.util.List;
@JsonPropertyOrder({ "Carpet", "Hard","Dusty" })
public class FloorDistribution {
    private double Carpet;
    private double Hard;
    private double Dusty;
    private List<Stats> stats;

    public FloorDistribution(List<Stats> stats) {
        this.stats=stats;

        long totalNum= stats.size();
        if (totalNum == 0) {
            // 데이터가 없으면 NaN 방지용 기본값
            this.Hard = 0;
            this.Carpet = 0;
            this.Dusty = 0;
            return; // 조기 종료
        }
        long hardNum= stats.stream().filter(s -> "Hard".equalsIgnoreCase(s.getStat())).count();
        long carpetNum = stats.stream().filter(s -> "Carpet".equalsIgnoreCase(s.getStat())).count();
        long dustyNum = stats.stream().filter(s -> "Dusty".equalsIgnoreCase(s.getStat())).count();

        this.Hard = ((double)hardNum/totalNum) * 100;
        this.Carpet = ((double)carpetNum/totalNum) * 100;
        this.Dusty = ((double)dustyNum/totalNum) * 100;
    }

    @JsonProperty("Carpet")
    public double getCarpet() {
        return Carpet;
    }

    @JsonProperty("Hard")
    public double getHard() {
        return Hard;
    }
    @JsonProperty("Dusty")
    public double getDusty() {
        return Dusty;
    }

}
