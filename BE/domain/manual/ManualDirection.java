package com.Study.vibeclean.domain.manual;


import jakarta.persistence.*;

@Entity
@Table(name="manual_direction")
public class ManualDirection {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    int id;

    @Column(nullable = false,name="direction")
    String direction;

    protected ManualDirection(){}

    public ManualDirection(String direction) {
        this.direction = direction;
    }

    public String getDirection() {
        return direction;
    }
}
