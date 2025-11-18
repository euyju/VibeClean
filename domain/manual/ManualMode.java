package com.Study.vibeclean.domain.manual;


import jakarta.persistence.*;

@Entity
@Table(name="manual_mode")
public class ManualMode {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    int id;

    @Column(nullable = false,name="mode")
    String mode;

    protected ManualMode(){}

    public ManualMode(String mode) {
        this.mode = mode;
    }

    public String getMode() {
        return mode;
    }
}
