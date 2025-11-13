package com.Study.vibeclean.dto.sensor.response;

public class SensorResponse {
    private int x;
    private int y;
    private int z;

    public SensorResponse(int x, int y, int z) {
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
