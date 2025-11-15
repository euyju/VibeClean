package com.Study.vibeclean.dto.manual.response;

public class ManualSpeedResponse {
    private int fanSpeed;

    public ManualSpeedResponse(int fanSpeed) {
        this.fanSpeed = fanSpeed;
    }

    public int getFanSpeed() {
        return fanSpeed;
    }
}
