package com.Study.vibeclean.domain.manual;

import jakarta.persistence.*;

@Entity
@Table(name = "manual_speed")
public class ManualSpeed {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @Column(nullable = false, name="speed")
    private int speed;

    protected ManualSpeed(){}

    public ManualSpeed(int speed) {
        this.speed = speed;
    }

    public int getSpeed() {
        return speed;
    }
}
