package com.Study.vibeclean.dto.service.status;

import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.repository.status.StatusRepository;
import com.Study.vibeclean.dto.status.request.Coordinate;
import com.Study.vibeclean.dto.status.request.RobotStatusRequest;
import com.Study.vibeclean.dto.status.response.RobotStatusResponse;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Service
public class StatusService {
    private StatusRepository repository;

    public StatusService(StatusRepository repository) {
        this.repository = repository;
    }

    @Transactional //postman에서 값을 전달하면 저장하는 기능
    public void saveStatus(RobotStatusRequest request){
        int x = request.getCoordinate().getX();
        int y = request.getCoordinate().getY();

        // 아래의 코드를 주석처리한 이유는, 기존의 DB에서 내가 별도로 localdate를 넣는 방향으로 최종 개발 진행중
        // 이 코드는 내가 postman활용해서 임시로 DB에 저장할 때 사용했던 거라서 사용할 일 없기도 하고 DB 셋팅도 다르니까 패스
        //repository.save(new Status(request.getPower(),request.getCurrentFloor(),request.getFanSpeed(),x,y));

    }

    @Transactional // 이 부분 짜는 거에서 살짝 막혔었다. postman을 활용해서 get으로 들어오면 아래의 상태 정보를 보내줌
    public RobotStatusResponse returnStatus(){
        Optional<Status> latestStatus= repository.findTopByOrderByIdDesc();//가장 최신의 값을 가져옴.
        if (latestStatus.isEmpty()){
            return new RobotStatusResponse("OFF",null,0,new ArrayList<>());
        }
        List<Coordinate> path= repository.findAllPathPoints();

        return new RobotStatusResponse(latestStatus.get().getPower(), latestStatus.get().getCurrentFloor(), latestStatus.get().getFanSpeed(),path);
    }

}
