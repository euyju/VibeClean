package com.Study.vibeclean.dto.mqtt;

import lombok.Data;

@Data
public class TelemetryMessage {

    private String currentFloor;
    private int fanSpeed;

    private Position position;
    private SensorData sensor;

    @Data
    public static class Position {
        private int x;
        private int y;

        public int getX() {
            return x;
        }

        public int getY() {
            return y;
        }
    }

    @Data
    public static class SensorData {
        private float x;
        private float y;
        private float z;

        public float getX() {
            return x;
        }

        public float getY() {
            return y;
        }

        public float getZ() {
            return z;
        }
    }

    public String getCurrentFloor() {
        return currentFloor;
    }

    public int getFanSpeed() {
        return fanSpeed;
    }

    public Position getPosition() {
        return position;
    }

    public SensorData getSensor() {
        return sensor;
    }
}