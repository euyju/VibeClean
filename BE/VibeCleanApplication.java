package com.Study.vibeclean;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

@EnableScheduling
@SpringBootApplication
public class VibeCleanApplication {

	public static void main(String[] args) {
		SpringApplication.run(VibeCleanApplication.class, args);
	}

}
