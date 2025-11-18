/*
package com.Study.vibeclean.controller.stm;

import com.Study.vibeclean.dto.service.stm.StmService;
import com.Study.vibeclean.dto.stm.request.StmRequest;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class StmController {
    private StmService stmService;

    public StmController(StmService stmService) {
        this.stmService = stmService;
    }

    @PostMapping("/api/send/value")
    public void saveValue(@RequestBody StmRequest request){
        stmService.saveValue(request);
    }
}
*/
