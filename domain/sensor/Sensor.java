package com.Study.vibeclean.domain.sensor;

import jakarta.persistence.*;

@Entity
public class Sensor {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @Column(nullable = false, name="x") // 여기도 이제 아래로 다 double로 바꿔주었다. 내가 이 센서 값들을 그렇게 바꾸었기 때문
    private float x;

    @Column(nullable = false, name="y")
    private float y;

    @Column(nullable = false, name="z")
    private float z;

    protected Sensor(){} // jpa를 사용하는 경우 이렇게 entity 객체에 기본적인 protected 생성자를 사용해야 한다.

    public Sensor(float x, float y, float z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

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
