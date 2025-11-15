package com.Study.vibeclean.controller.manual;

import com.Study.vibeclean.dto.manual.request.ManualPowerRequest;
import com.Study.vibeclean.dto.manual.request.ManualSpeedRequest;
import com.Study.vibeclean.dto.manual.response.ManualPowerResponse;
import com.Study.vibeclean.dto.manual.response.ManualSpeedResponse;
import com.Study.vibeclean.dto.service.manual.ManualControlService;
import lombok.RequiredArgsConstructor;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RequiredArgsConstructor
@RestController
public class ManualController {
    private final ManualControlService manualControlService;

    @PostMapping("/api/manual/speed")
    public void setSpeed(@RequestBody ManualSpeedRequest request){
        manualControlService.setSpeed(request);
    }

    @PostMapping("/api/manual/power")
    public void setPower(@RequestBody ManualPowerRequest request){
        manualControlService.setPower(request);
    }

    @GetMapping("/api/manual/speed")
    public ManualSpeedResponse getSpeed(){ return manualControlService.getSpeed();
    }

    @GetMapping("/api/manual/power")
    public ManualPowerResponse getPower(){ return manualControlService.getPower();
    }


}
