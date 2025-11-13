package com.Study.vibeclean.dto.sensor.request;

public class SensorRequest {
    private int x;
    private int y;
    private int z;

    public SensorRequest(int x, int y, int z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public int getZ() {
        return z;
    }
}
