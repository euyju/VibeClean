package com.Study.vibeclean.domain.manual;

import jakarta.persistence.*;

@Entity
@Table(name = "manual_power")
public class ManualPower {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @Column(nullable = false, name="power")
    private String power;

    protected ManualPower(){}

    public ManualPower(String power) {
        this.power = power;
    }

    public String getPower() {
        return power;
    }
}
