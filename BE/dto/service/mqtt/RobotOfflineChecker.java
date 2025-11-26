package com.Study.vibeclean.dto.service.mqtt;

import com.Study.vibeclean.domain.manual.ManualDirection;
import com.Study.vibeclean.domain.manual.ManualMode;
import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.repository.manual.ManualDirectionRepository;
import com.Study.vibeclean.dto.repository.manual.ManualModeRepository;
import com.Study.vibeclean.dto.repository.manual.ManualPowerRepository;
import com.Study.vibeclean.dto.repository.manual.ManualSpeedRepository;
import com.Study.vibeclean.dto.repository.sensor.SensorRepository;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Slf4j
@Service
@EnableScheduling // 스프링 부트에서 주기적인 작업(스케쥴러)를 실행하기 위해 필요한 어노테이션
@RequiredArgsConstructor
public class RobotOfflineChecker {
    private final StatusRepository statusRepository;
    private final SensorRepository sensorRepository;
    private final ManualSpeedRepository manualSpeedRepository;
    private final ManualPowerRepository manualPowerRepository;
    private final ManualModeRepository manualModeRepository;
    private final ManualDirectionRepository manualDirectionRepository;

    @Scheduled(fixedDelay = 1000) // 이거를 통해 스프링부트가 1초마다 반복 실행되게 만든다.
    public void checkOffline() {
        Status latest = statusRepository.findTopByOrderByTimeDesc();
        if (latest == null) {
            return; // 이미 초기 상태
        }

        LocalDateTime now = LocalDateTime.now();
        if (latest.getTime().isBefore(now.minusSeconds(59))) { // 가장 최근에 status에 저장된 시간과 현재 시간이 3초가 넘으면 일어나는 일
            // 지금은 테스트 단계라서 59초로 설정했지만, 실제로 할 때는 3초나 더 간격 작게 해서 고고링
            log.info("No telemetry for 3s → Robot OFF → clear tables");
            statusRepository.deleteAll();
            sensorRepository.deleteAll();// 꺼졌다고 판단하여 로그에 출력 및 db에 저장된 모든 값들을 삭제한다.
            manualPowerRepository.deleteAll();
            manualSpeedRepository.deleteAll();
            manualDirectionRepository.deleteAll();
            manualModeRepository.deleteAll();
            manualModeRepository.save(new ManualMode("AUTO")); // 수동 조작을 할 때, mode를 다루는 경우, 기본 초기화 값 자체가 auto가 들어간 상태이므로,
            // 모든 행을 다 삭제한 후에, 기본 값인 AUTO를 넣어둔다.
        }
    }
}
