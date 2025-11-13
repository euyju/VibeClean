package com.Study.vibeclean.dto.manual.request;

public class ManualSpeedRequest {
    private int fanSpeed;

    public ManualSpeedRequest(int fanSpeed) {
        this.fanSpeed = fanSpeed;
    }

    public int getFanSpeed() {
        return fanSpeed;
    }
}
