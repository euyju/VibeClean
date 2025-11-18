package com.Study.vibeclean.dto.status.response;

import com.Study.vibeclean.dto.status.request.Coordinate;

import java.util.List;

public class RobotStatusResponse {
    private String power;
    private String mode;
    private String currentFloor;
    private int fanSpeed;
    private List<Coordinate> pathHistory; // 이 부분에 x,y 좌표를 받을 예정

    public RobotStatusResponse(String power, String mode, String currentFloor, int fanSpeed, List<Coordinate> pathHistory) {
        this.power = power;
        this.mode = mode;
        this.currentFloor = currentFloor;
        this.fanSpeed = fanSpeed;
        this.pathHistory = pathHistory;
    }

    public String getPower() {
        return power;
    }

    public String getMode() {
        return mode;
    }

    public String getCurrentFloor() {
        return currentFloor;
    }

    public int getFanSpeed() {
        return fanSpeed;
    }

    public List<Coordinate> getPathHistory() {
        return pathHistory;
    }

}
