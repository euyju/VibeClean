package com.Study.vibeclean.dto.status.request;
// 현재는 STM쪽에서 직접적으로 통신이 불가능하기에, 우선적으로 postman으로 api통해서 값이 잘 입력되는지 확인하기 위한 임시 request

public class RobotStatusRequest {
    private String power;
    private String currentFloor;
    private int fanSpeed;
    private Coordinate coordinate; // 이 부분에 x,y 좌표를 받을 예정


    public RobotStatusRequest(String power, String currentFloor, int fanSpeed, Coordinate coordinate) {
        this.power = power;
        this.currentFloor = currentFloor;
        this.fanSpeed = fanSpeed;
        this.coordinate = coordinate;

    }

    public String getPower() {
        return power;
    }

    public String getCurrentFloor() {
        return currentFloor;
    }

    public int getFanSpeed() {
        return fanSpeed;
    }

    public Coordinate getCoordinate() {
        return coordinate;
    }


}
