package com.Study.vibeclean.dto.repository.status;

import com.Study.vibeclean.domain.status.Status;
import com.Study.vibeclean.dto.status.request.Coordinate;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.List;
import java.util.Optional;

public interface StatusRepository extends JpaRepository <Status,Long> {
    Status findTopByOrderByTimeDesc();
    Status findTopByOrderByIdAsc();
    long count();
    Optional<Status> findTopByOrderByIdDesc();
    @Query("""
   select new com.Study.vibeclean.dto.status.request.Coordinate(s.x, s.y)
   from Status s
   order by s.id asc
   """)
    List<Coordinate> findAllPathPoints();
}
