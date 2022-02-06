# CPU-Scheduling-Simulator
> 2019년 1학기 운영체제 수업에서 텀프로젝트로 진행했던 CPU Scheduling Simulator.
----
## Term Project 의 목적 
- OS 의 기본 개념을 바탕으로 CPU 스케줄러의 기능 및 작동 원리를 이해한다.

----
## Language
- C

----
## CPU Scheduling Simulator 의 구성
#### Create_Process(): 실행할 프로세스를 생성하고 각 프로세스에 데이터가 주어진다. (Random data 부여)
- Process ID
- CPU burst time
- I/O burst time
- Arrival time
- Priority

#### Config(): 시스템 환경 설정
- Ready Queue / Waiting Queue

####  Schedule(): CPU 스케줄링 알고리즘을 구현한다.
- FCFS(First Come First Served)
- SJF(Shortest Job First)
- Priority
- RR(Round Robin)
- Preemptive 방식 적용 – SJF, Priority

#### Evaluation(): 각 스케줄링 알고리즘들간 비교 평가한다.
- Average waiting time
- Average turnaround time


