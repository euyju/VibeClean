package com.Study.vibeclean.dto.status.request;
// x,y좌표 값을 입력받기 위한 클래스

public class Coordinate {
    private int x;
    private int y;

    public Coordinate(int x, int y) {
        this.y = y;
        this.x = x;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }
}
