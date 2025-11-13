package com.Study.vibeclean.controller.status;

import com.Study.vibeclean.dto.service.manual.ManualControlService;
import com.Study.vibeclean.dto.service.status.StatusService;
import com.Study.vibeclean.dto.status.request.RobotStatusRequest;
import com.Study.vibeclean.dto.status.response.RobotStatusResponse;
import lombok.RequiredArgsConstructor;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class StatusController {
    private StatusService statusService;

    public StatusController(StatusService statusService) {
        this.statusService = statusService;
    }

    //@PostMapping("/api/robot/status")
    //public void RobotStatus(@RequestBody RobotStatusRequest request){
        //manualControlService.setSpeed(request);
        //statusService.saveStatus(request); //현재 토픽 받으면 바로 저장되게 해놨으므로 이 코드는 굳이 필요 없음
    //}

    @GetMapping("/api/robot/status")
    public RobotStatusResponse SendRobotStatus(){
        RobotStatusResponse responses=statusService.returnStatus();
        return responses;

    }
}
