#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define NUM_PROCESS 5  //PROCESS 개수 5개 고정

typedef struct process
{
	int PID;	// process ID
	int cpuburst;	// cpu burst 시간
	int cpuburst_beforeio;	// io 작업 전에 필요한 cpu 작업 시간
	int ioburst;	// io 작업 시간
	int arrival;	// 프로세스가 ready queue에 도착한시간(but, 도착한시간대로 ready queue에 적재하는 작업을 따로 해줘야함)
	int priority;	// process의 우선순위
}Process;

typedef struct node // 각 프로세스의 노드
{
	Process ps;	// 프로세스
	int index;	// queue 안에서의 index
	struct node* next;	// 다음 프로세스
}NODE;

typedef struct start	// 앞으로 사용할 ready queue나 waiting queue의 시작점
{
	int count;	// queue의 전체 개수
	struct start* next;	// queue의 첫번째 process를 가리킴
}SNODE;


int Create_Process(Process * ps);	// 프로세스 랜덤생성
Process Create_Null_Process();	// NULL 프로세스 생성 
int Create_Process_Self(Process * ps);	// 프로세스 수동생성
int Print_Process(Process * ps);	// 프로세스 출력
int Copy_Process(Process * ps1, Process *ps2);	// 프로세스 복사


int FCFS(Process ps, SNODE *q);
int SJF(Process ps, SNODE *q);
int Pre_SJF(Process ps, SNODE *q, Process *run);
int Priority(Process ps, SNODE *q);
int Pre_Priority(Process ps, SNODE *q, Process *run);
int RR(Process ps, SNODE *q);


int running(Process *ps);	// 전체 실행
int Select_AlgoNum();	// 전체 실행 내부에서 알고리즘 선택에 따른 실행
int Process_Scheduling(int schnum, Process *ps, double * average);	// 각 알고리즘 스케줄러에 따라 프로세스 처리
int Print_Time(int tw[][5], double*average);	// 스케줄링이 끝났을 때, 각 프로세스의 turnaround, waiting time 계산
int Print_Evaluation(double scheduling_aveTW[][6]);	// 전체 알고리즘 실행이 끝났을 때, 알고리즘 별 평균 turnaround, waiting time 출력 및 비교

SNODE Create_SNODE();	// queue의 시작을 만드는 함수
int addLast(Process ps, SNODE *q);	// queue의 리스트 마지막(오른쪽방향)끝에 프로세스를 추가하는 함수
int addWant(Process ps, int index, SNODE *q);	// queue의 리스트 원하는 위치에 프로세스를 추가하는 함수
Process getLast(SNODE *q);	// queue의 리스트에서 가장 마지막(오른쪽방향)끝에 있는 프로세스를 반환하는 함수
Process getWant(int index, SNODE *q);	// queue의 리스트에서 원하는 위치의 프로세스를 반환하는 함수


//-----------------------------------------------main 함수------------------------------------------------------------------------------------------------------		
int main()
{
	int sel = 0;

	printf("CPU Simulater는 5개의 process를 통해 스케줄링 알고리즘을 시뮬레이션합니다.\n");
	printf("참고사항 : 괄호 안의 숫자는 해당 시점의 time입니다!\n");

	while (1)	// 프로그램 실행
	{
		printf("\n\n1. Run Simulator\n2. Quit\n");
		scanf("%d", &sel);

		if (sel == 1)	// Run Simulator
		{
			Process ps[NUM_PROCESS];	// 프로세스 생성 단계

			while (1)
			{
				printf("\n1. 프로세스 랜덤 생성\n2. 프로세스 직접 생성\n");
				scanf("%d", &sel);

				if (sel == 1)
				{
					Create_Process(ps);	// 랜덤생성
					break;
				}
				else if (sel == 2)
				{
					Create_Process_Self(ps);	// 수동생성
					break;
				}
				else
					printf("잘못 입력하셨습니다.\n");
			}

			printf("\nProcess들을 생성중입니다.\n");
			usleep(1000*500);
			printf("생성완료\n");
			usleep(1000*500);
			printf("Process들을 출력합니다.\n\n");
			usleep(1000*500);

			running(ps);	// 스케줄링 실행
		}

		else if (sel == 2)	// Quit
			break;

		else
		{
			printf("잘못 입력하셨습니다.\n");
			continue;
		}
	}

	printf("\n잠시 후 Simulater를 종료합니다.\n");	// sel 2 선택시 프로그램 종료

	for (int i = 3; i > 0; i--)
	{
		printf("%d\n", i);
		usleep(1000*1000);
	}
	printf("종료\n");

	return 0;
}


//-----------------------------------------------프로세스 관련 함수-----------------------------------------------------------------------------------------------
int Create_Process(Process * ps)	// 프로세스 랜덤생성
{
	int i;

	for (i = 0; i < NUM_PROCESS; i++)	// 프로세스 구성 요소 랜덤 초기화
	{
		ps[i].PID = i + 1;
		ps[i].cpuburst = rand() % 10 + 3;
		ps[i].ioburst = rand() % 10;

		if (ps[i].ioburst > 0)
			ps[i].cpuburst_beforeio = ps[i].cpuburst - 2;
		else
			ps[i].cpuburst_beforeio = 0;

		ps[i].arrival = rand() % 8;
		ps[i].priority = rand() % 5;
	}

	return 0;
}

Process Create_Null_Process()	// 프로세스 구성 요소 0으로 초기화
{
	Process ps;
	ps.PID = 0;
	ps.cpuburst = 0;
	ps.ioburst = 0;
	ps.cpuburst_beforeio = 0;
	ps.arrival = 0;
	ps.priority = 0;
	return ps;
}

int Create_Process_Self(Process * ps)	// 프로세스 수동생성
{
	int i;
	printf("\n모든 값은 100을 초과할 수 없습니다.\n");

	for (i = 0; i < NUM_PROCESS; i++)
	{
		ps[i].PID = i + 1;
		printf("\n%d번 프로세스의 cpu burst 값을 입력하세요 : ", i + 1);
		scanf("%d", &ps[i].cpuburst);

		while (ps[i].cpuburst > 100)
		{
			printf("Error : cpu burst 값이 100을 초과했습니다.\n");
			printf("\n%d번 프로세스의 cpu burst 값을 다시 입력하세요 : ", i + 1);
			scanf("%d", &ps[i].cpuburst);
		}

		printf("%d번 프로세스의 io burst 값을 입력하세요 : ", i + 1);
		scanf("%d", &ps[i].ioburst);

		while (ps[i].ioburst>100)
		{
			printf("Error : io burst 값이 100을 초과했습니다.\n");
			printf("\n%d번 프로세스의 io burst 값을 다시 입력하세요 : ", i + 1);
			scanf("%d", &ps[i].ioburst);
		}

		while (ps[i].ioburst>0 && ps[i].cpuburst<3)
		{
			printf("Error : ioburst를 처리하기 위해서는 cpuburst가 최소 3 이상 필요합니다.\n");
			printf("\n%d번 프로세스의 cpu burst 값을 다시 입력하세요 : ", i + 1);
			scanf("%d", &ps[i].cpuburst);
		}

		if (ps[i].ioburst>0)
			ps[i].cpuburst_beforeio = ps[i].cpuburst - 2;
		else
			ps[i].cpuburst_beforeio = 0;

		printf("%d번 프로세스의 arrival time 값을 입력하세요 : ", i + 1);
		scanf("%d", &ps[i].arrival);

		while (ps[i].arrival>100)
		{
			printf("Error : arrival 값이 100을 초과했습니다.\n");
			printf("\n%d번 프로세스의 arrival 값을 입력하세요 : ", i + 1);
			scanf("%d", &ps[i].arrival);
		}

		printf("%d번 프로세스의 priority 값을 입력하세요 : ", i + 1);
		scanf("%d", &ps[i].priority);

		while (ps[i].ioburst>100)
		{
			printf("Error : priority 값이 100을 초과했습니다.\n");
			printf("\n%d번 프로세스의 priority 값을 입력하세요 : ", i + 1);
			scanf("%d", &ps[i].priority);
		}
	}

	return 0;
}

int Print_Process(Process * ps)	// 프로세스 출력
{
	int i;
	printf("--------------------------------------------------------------------------\n");
	printf("PID |cpuburst\t|cpuburst_beforeio\t|ioburst   |arrivaltime\t|priority\n");

	for (i = 0; i < NUM_PROCESS; i++)
		printf("%d   |%d\t\t|%d\t\t\t|%d\t   |%d\t\t|%d\n", ps[i].PID, ps[i].cpuburst, ps[i].cpuburst_beforeio, ps[i].ioburst, ps[i].arrival, ps[i].priority);

	printf("--------------------------------------------------------------------------\n");

	return 0;
}

int Copy_Process(Process * ps1, Process *ps2)	// 프로세스 복사
{
	int i;

	for (i = 0; i < NUM_PROCESS; i++)
	{
		ps2[i].PID = ps1[i].PID;
		ps2[i].cpuburst = ps1[i].cpuburst;
		ps2[i].ioburst = ps1[i].ioburst;
		ps2[i].cpuburst_beforeio = ps1[i].cpuburst_beforeio;
		ps2[i].arrival = ps1[i].arrival;
		ps2[i].priority = ps1[i].priority;
	}

	return 0;
}

//-----------------------------------------------스케줄러 함수---------------------------------------------------------------------------------------------------

int FCFS(Process ps, SNODE *q)
{
	addWant(ps, 0, q); // 항상 index 0번째로 보냄
	return 0;
}

int SJF(Process ps, SNODE *q)
{
	NODE *a = (NODE *)q->next;
	int index = -1;

	while (a != NULL)	// 새로 들어온 프로세스를 기존 ready 내의 process 들과 비교하여 적절한 위치에 삽입. 각 프로세스의 cpuburst_beforeio 존재 여부에 따라 4가지 경우.
	{
		if (ps.cpuburst_beforeio > 0)	// 새로 들어온 프로세스의 cpuburst_beforeio
		{
			if (a->ps.cpuburst_beforeio > 0)	// ready queue가 가리키는 프로세스의 cpuburst_beforeio
			{
				if (a->ps.cpuburst_beforeio > ps.cpuburst_beforeio)
					index = a->index;
			}
			else
			{
				if (a->ps.cpuburst > ps.cpuburst_beforeio)
					index = a->index;
			}
		}
		else
		{
			if (a->ps.cpuburst_beforeio > 0)
			{
				if (a->ps.cpuburst_beforeio > ps.cpuburst)
					index = a->index;
			}
			else
			{
				if (a->ps.cpuburst > ps.cpuburst)
					index = a->index;
			}
		}
		a = a->next;
	}
	if (q->count == index + 1)	// 새로 들어온 프로세스와 ready queue에 있는 프로세스들의 cpuburst를 비교하고, 새로 들어온 프로세스의 cpuburst가 더 작은 경우
		addLast(ps, q);
	else
		addWant(ps, index + 1, q);	// 여기서 index는 기존에 ready queue에 있던 프로세스의(P1) cpuburst 또는 cpuburst_beforeio 값이 새로 들어온 프로세스의(P2) 값보다 커서 대입된 P1의 index값이다.
									// index에 1을 더해서 인자로 보내는 이유는 ready queue에서 P2를 P1 다음으로 링크하기 위해서이다. (리스트에서 더 오른쪽 방향에 있을 수록 run queue로 가는 순서가 빠르기 때문) 
	return 0;
}

int Pre_SJF(Process ps, SNODE *q, Process *run)
{
	NODE *a = (NODE *)q->next;
	int index = -1;
	int run_preemptivecheck = 0; // 1이면 run queue에 있던 프로세스가 새로 들어온 프로세스한테 선점당하고, 0이면 안당함

	while (a != NULL)
	{
		if (ps.cpuburst_beforeio > 0)
		{
			if (a->ps.cpuburst_beforeio > 0)
			{
				if (a->ps.cpuburst_beforeio > ps.cpuburst_beforeio)
					index = a->index;
			}
			else
			{
				if (a->ps.cpuburst > ps.cpuburst_beforeio)
					index = a->index;
			}
		}

		else
		{
			if (a->ps.cpuburst_beforeio > 0)
			{
				if (a->ps.cpuburst_beforeio > ps.cpuburst)
					index = a->index;
			}
			else
			{
				if (a->ps.cpuburst > ps.cpuburst)
					index = a->index;
			}
		}
		a = a->next;
	}

	if (q->count == index + 1)	// 새로 들어온 프로세스와 ready queue에 있는 프로세스들의 cpuburst를 비교하고, 새로 들어온 프로세스의 cpuburst가 더 작은 경우
	{
		if (ps.cpuburst_beforeio > 0)
		{
			if (run->cpuburst_beforeio > 0)
			{
				if (run->cpuburst_beforeio > ps.cpuburst_beforeio)	// run queue에서 작업하고 있는 프로세스와 남은 cpuburst양을 비교
					run_preemptivecheck = 1;
			}
			else
			{
				if (run->cpuburst > ps.cpuburst_beforeio)
					run_preemptivecheck = 1;
			}
		}

		else
		{
			if (run->cpuburst_beforeio > 0)
			{
				if (run->cpuburst_beforeio > ps.cpuburst)
					run_preemptivecheck = 1;
			}
			else
			{
				if (run->cpuburst > ps.cpuburst)
					run_preemptivecheck = 1;
			}
		}

		if (run->PID > 0 && run_preemptivecheck == 1)
		{
			addLast(ps, q); // 새로들어온 프로세스의 cpuburst가 run queue에있는 프로세스보다 cpuburst가 작은 경우 일단 ready queue 끝에 추가시켜놓음
			return run_preemptivecheck;
		}
		else
			addLast(ps, q);	 // 1. 첫번째로 도착한 프로세스를 ready queue에 적재하는 경우
							 // 2. 새로 들어온 프로세스의 cpuburst가 ready queue에 있는 프로세스들의 cpuburst값보다 작지만 run queue에 있는 프로세스의 cpuburst보다 큰 경우(선점못함)
							 // 3. 새로 들어온 프로세스의 cpuburst가 ready queue에 있는 프로세스들의 cpuburst값보다 작은 것을 확인하고 이제 run queue에 있는 프로세스와 비교하려했더니
							 // 원래는 안 그래야 되는데 작성한 코드 순서상 run queue가 비워져있는 경우

	}

	else  // 새로 들어온 프로세스의 cpuburst가 ready queue에 있던 프로세스의 cpuburst보다 큰 경우
		addWant(ps, index + 1, q);

	return 0;
}

int Priority(Process ps, SNODE *q)	// 동등한 우선순위 일때는 FCFS 적용!
{
	NODE *a = (NODE *)q->next;
	int index = -1;

	while (a != NULL)
	{
		if (a->ps.priority > ps.priority)
			index = a->index;
		a = a->next;
	}

	if (q->count == index + 1) // 1. 첫 프로세스가 도착해서 ready queue에 적재하는 경우
		addLast(ps, q);	       // 2. 새로 들어온 프로세스의 우선순위가 기존에 ready queue에 있던 프로세스들보다 우선순위가 높은 경우
	else
		addWant(ps, index + 1, q);

	return 0;
}

int Pre_Priority(Process ps, SNODE *q, Process *run)
{
	NODE *a = (NODE *)q->next;
	int index = -1;
	int run_preemptivecheck = 0;

	while (a != NULL)
	{
		if (a->ps.priority > ps.priority)
			index = a->index;
		a = a->next;
	}

	if (q->count == index + 1)
	{
		if (run->PID > 0 && run->priority > ps.priority)  // "run->PID > 0" 도 같이 검사하는 이유는 run queue에 프로세스가 존재하는데 run->priority가 0인 경우가 존재할 수 있어 run queue가 비어있는 걸로 인식되는 것을 방지
			run_preemptivecheck = 1;

		if (run_preemptivecheck == 1)
		{
			addLast(ps, q);
			return run_preemptivecheck;
		}
		else
			addLast(ps, q);
	}
	else
		addWant(ps, index + 1, q);

	return 0;
}

int RR(Process ps, SNODE *q)
{
	addWant(ps, 0, q);	// index를 0으로 설정한 이유는 FCFS를 적용하기 위해
	return 0;
}


//------------------------------------------------전체 실행 함수------------------------------------------------------------------------------------------------
int running(Process *ps)
{
	double average[2];
	double scheduling_aveTW[2][6];	// 6개의 스케줄링기법 각각의 turnaround, waiting time의 평균값이 저장됨
	int check[6] = { 0 };	// 스케줄링 기법은 각각 한번씩만 이용할 수 있음
	int scheduling_count = 0;	// 스케줄링 기법 6개

	while (scheduling_count < 6)
	{
		Print_Process(ps);	// 5개의 프로세스 출력

		switch (Select_AlgoNum())	// 스케줄링 기법 번호 선택
		{
		case 1:
			if (check[0] == 0)
			{
				Process_Scheduling(1, ps, average);
				scheduling_aveTW[0][0] = average[0];	// turnaround time의 평균
				scheduling_aveTW[1][0] = average[1];	// waiting time의 평균
				scheduling_count++;
				check[0] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n\n");
				break;
			}
		case 2:
			if (check[1] == 0)
			{
				Process_Scheduling(2, ps, average);
				scheduling_aveTW[0][1] = average[0];
				scheduling_aveTW[1][1] = average[1];
				scheduling_count++;
				check[1] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n");
				break;
			}
		case 3:
			if (check[2] == 0)
			{
				Process_Scheduling(3, ps, average);
				scheduling_aveTW[0][2] = average[0];
				scheduling_aveTW[1][2] = average[1];
				scheduling_count++;
				check[2] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n");
				break;
			}
		case 4:
			if (check[3] == 0)
			{
				Process_Scheduling(4, ps, average);
				scheduling_aveTW[0][3] = average[0];
				scheduling_aveTW[1][3] = average[1];
				scheduling_count++;
				check[3] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n");
				break;
			}
		case 5:
			if (check[4] == 0)
			{
				Process_Scheduling(5, ps, average);
				scheduling_aveTW[0][4] = average[0];
				scheduling_aveTW[1][4] = average[1];
				scheduling_count++;
				check[4] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n");
				break;
			}
		case 6:
			if (check[5] == 0)
			{
				Process_Scheduling(6, ps, average);
				scheduling_aveTW[0][5] = average[0];
				scheduling_aveTW[1][5] = average[1];
				scheduling_count++;
				check[5] = 1;
				break;
			}
			else
			{
				printf("이미 수행한 알고리즘입니다.\n");
				break;
			}
		case 7:
			return 0;
		}
	}

	printf("\n모든 스케줄링 알고리즘을 사용하였습니다.\n");
	usleep(1000*500);
	printf("모든 결과를 불러오는 중입니다.\n잠시만 기다려주세요!\n\n");
	usleep(1000*500);
	printf("--------FCFS 불러오는 중----------------------------\n");
	usleep(1000*500);
	printf("--------Non-Preemptive SJF 불러오는 중--------------\n");
	usleep(1000*500);
	printf("--------Preemptive SJF 불러오는 중------------------\n");
	usleep(1000*500);
	printf("--------Non-Preemptive Priority 불러오는 중---------\n");
	usleep(1000*500);
	printf("--------Preemptive Priority 불러오는 중-------------\n");
	usleep(1000*500);
	printf("--------Round Robin 불러오는 중---------------------\n");
	usleep(1000*500);

	printf("\n");
	Print_Evaluation(scheduling_aveTW);
	return 0;
}

int Select_AlgoNum()
{
	int sel;
	printf("\n\nCPU Scheduling 알고리즘을 선택해주세요.\n");
	printf("1. FCFS\n");
	printf("2. SJF\n");
	printf("3. Preemptive SJF\n");
	printf("4. Priority\n");
	printf("5. Preemptive Priority\n");
	printf("6. RR\n");
	printf("7. reset\n");
	scanf("%d", &sel);
	return sel;
}

//-----------------------------------------------실질적 실행 함수------------------------------------------------------------------------------------------------
int Process_Scheduling(int schnum, Process *ps, double*average)
{
	int process_count = 0;
	int time = 0;
	int quantum = 0;
	int temp_quantum = 0;

	int preemptiveCheck1 = 0; // 1이면 선점당하고 0이면 안당함
	int temp_preemptiveCheck1 = 0;
	int preemptiveCheck2 = 0;
	int temp_preemptiveCheck2 = 0;

	Process temp_process1 = Create_Null_Process();
	Process temp_process2 = Create_Null_Process();
	Process temp_process3 = Create_Null_Process();

	int tw[2][5]; // 각 프로세스의 turnaround,waiting time을 저장

	Process run = Create_Null_Process();

	SNODE ready;
	ready = Create_SNODE();
	SNODE wait;
	wait = Create_SNODE();
	NODE *temp;

	if (schnum == 6)
	{
		printf("\nTime Quantum을 입력해주세요!\n");
		scanf("%d", &quantum);
		temp_quantum = quantum;
	}

	Process copy[NUM_PROCESS];
	Copy_Process(ps, copy);
	printf("\n<간트차트>\n");
	printf("/(%d)/ ", time);	// 첫 0초 출력


	while (process_count < NUM_PROCESS)	// 모든 프로세스가 terminate 될 때까지 run
	{
		temp_process1 = Create_Null_Process();
		temp_process2 = Create_Null_Process();
		temp_process3 = Create_Null_Process();
		//arrivial time과 time(현재시간)을 비교하여 ready queue에 process 올리기
		for (int i = 0; i < 5; i++) // Ex) 맨처음에 FCFS를 선택하고 arrival이 0인 프로세스만 수행(이때 time도 0)
		{
			if (copy[i].arrival == time) // time은 흘러가는 시간(0초부터 1초씩 증가), ready queue에 도착한시간(copy[i].arrival)대로 ready queue에 적재할 준비함
			{
				switch (schnum)
				{
				case 1:
					FCFS(copy[i], &ready); // 프로세스 i번째 ready queue로 적재하러~
					break;
				case 2:
					SJF(copy[i], &ready);
					break;
				case 3:
					preemptiveCheck1 = Pre_SJF(copy[i], &ready, &run); // preemptiveCheck에 0 또는 1 리턴
					temp_preemptiveCheck1 = preemptiveCheck1;
					if (temp_preemptiveCheck1 == 1) // 새로 들어온 프로세스의 cpuburst가 run queue에서 작업중이던 프로세스의 cpuburst보다 작은경우 => 선점!
					{								// 같은 시간때에 waitqueue에서 작업을 마치고 나온 프로세스의 경우 선점이 안되는 상황 방지
						temp_process1 = run;
						run = getLast(&ready);
						if (temp_process1.PID != 0)
							Pre_SJF(temp_process1, &ready, &run);
						printf("/(%d)/ ", time);
						temp_preemptiveCheck1 = 0;
						preemptiveCheck1 = 0;
					}
					break;
				case 4:
					Priority(copy[i], &ready);
					break;
				case 5:
					preemptiveCheck1 = Pre_Priority(copy[i], &ready, &run);
					temp_preemptiveCheck1 = preemptiveCheck1;
					if (temp_preemptiveCheck1 == 1)
					{
						temp_process1 = run;
						run = getLast(&ready);
						if (temp_process1.PID != 0)
							Pre_Priority(temp_process1, &ready, &run);
						printf("/(%d)/ ", time);
						temp_preemptiveCheck1 = 0;
						preemptiveCheck1 = 0;
					}
					break;
				case 6:
					RR(copy[i], &ready);
					break;
				}
			}
		}

		if (run.PID == 0)	// run queue에 프로세스가 적재되지 않았을 때(ready queue가 빈 경우)
		{
			run = getLast(&ready); // ready queue끝에 있는 프로세스를 run queue에 적재

			if (schnum == 6 && run.PID != 0)
				quantum = temp_quantum;
		}


		if (wait.count > 0)	// waiting queue 구현, wait queue에 대기하고 있는 프로세스 존재
		{
			temp = (NODE *)wait.next;	// temp는 wait queue에 있는 index 0인 프로세스를 가리킴

			do {
				temp->ps.ioburst--;

				if (temp->ps.ioburst == 0)
				{
					int temp_index = temp->index;
					temp = temp->next;
					switch (schnum)
					{
					case 1:
						FCFS(getWant(temp_index, &wait), &ready);
						break;
					case 2:
						SJF(getWant(temp_index, &wait), &ready);
						break;
					case 3:
						temp_process2 = getWant(temp_index, &wait);
						preemptiveCheck2 = Pre_SJF(temp_process2, &ready, &run);	// waitqueue에서 작업을 마치고 나온 프로세스를 ready queue와 run queue에 있는 프로세스들과 비교
						if (preemptiveCheck2 == 1)
						{
							temp_preemptiveCheck2 = preemptiveCheck2;
							preemptiveCheck2 = 0;
						}
						break;
					case 4:
						Priority(getWant(temp_index, &wait), &ready);
						break;
					case 5:
						temp_process2 = getWant(temp_index, &wait);
						preemptiveCheck2 = Pre_Priority(temp_process2, &ready, &run);
						if (preemptiveCheck2 == 1)
						{
							temp_preemptiveCheck2 = preemptiveCheck2;
							preemptiveCheck2 = 0;
						}
						break;
					case 6:
						RR(getWant(temp_index, &wait), &ready);
						break;
					}
				}
				else
					temp = temp->next;

			} while (temp != NULL);
		}


		if (run.PID == 0)	// ready queue가 빈 경우
		{
			printf("(idle) ");
			printf("/(%d)/ ", time + 1);

		}
		else if (run.cpuburst_beforeio != 0)
		{
			run.cpuburst_beforeio--;
			run.cpuburst--;

			if (schnum == 6)
				quantum--;

			 printf("%d ", run.PID); // Ex) FCFS일때 Run queue에 있는 PID 한번 출력하고 이 else if문 빠져나가서 맨 아래있는 time++수행
											 //     그리고 맨위로 다시가서 for문을 만나면 copy[i].arrival과 증가된 time값이랑 일치하는 프로세스가 있는지 확인(나머지 프로세스가 어느 시간때에 ready queue에 도착했는지 확인(but, 아직 ready queue에 적재되기전)
											 //     일치된 프로세스가 있다면 FCFS(copy[i], &ready); 수행
			if (run.cpuburst_beforeio == 0)
			{
				addLast(run, &wait);	// run queue에 있던 프로세스 io작업하러 wait queue로 이동, 일단 무조건 wait queue 끝에 추가
				run = Create_Null_Process();	// run queue에 있던 프로세스가 wait queue로 이동했으니 run queue를 비우는 작업
				printf("/(%d)/ ", time + 1);	// time에 1을 더해주는 이유는 time++이 맨 아래에있어 간트차트에서의 시간흐름을 맞추려면 인위적으로 1을 더해줘야됨
			}
			else if (schnum == 6 && quantum == 0)
			{
				RR(run, &ready);
				run = Create_Null_Process();
				printf("/(%d)/ ", time + 1);
			}

		}
		else // run.cpuburst_beforeio == 0
		{
			run.cpuburst--;

			if (schnum == 6)
				quantum--;

			printf("%d ", run.PID);

			if (run.cpuburst == 0)	// 프로세스 할일 다함(terminated)
			{
				process_count++;
				tw[0][run.PID - 1] = time + 1 - run.arrival;	// turnaround time
				tw[1][run.PID - 1] = tw[0][run.PID - 1] - ps[run.PID - 1].cpuburst - ps[run.PID - 1].ioburst;	// waiting time
				printf("/(%d)/ ", time + 1);
				run = Create_Null_Process();
			}
			else if (schnum == 6 && quantum == 0)
			{
				RR(run, &ready);
				run = Create_Null_Process();
				printf("/(%d)/ ", time + 1);
			}
		}

		if (temp_preemptiveCheck2 == 1)	// wait queue에서 io작업을 마치고 run queue에 있는 프로세스를 선점하려고함, 코드의 시간흐름상 이 코드를 여기에 배치해야함
		{
			temp_process3 = run;
			run = getLast(&ready);

			if (schnum == 3 && temp_process3.PID != 0)	// "temp_process3.PID != 0" => 선점 작업을 하기전에 run queue가 미리 비어져있을 상황방지
				Pre_SJF(temp_process3, &ready, &run);	//  run queue에있던 프로세스 선점 당해서 ready queue로~

			else if (schnum == 5 && temp_process3.PID != 0)
				Pre_Priority(temp_process3, &ready, &run);

			printf("/(%d)/ ", time + 1);
			temp_preemptiveCheck2 = 0;
		}


		time++; // ※유의사항! => 위에 작업 다 수행하고 나서야 한발짝 늦게 time 1증가됨
	}

	printf("\n\n");
	Print_Time(tw, average);

	return 0;
}

int Print_Time(int tw[][5], double *average)
{
	double turnaround_sum = 0;
	double wait_sum = 0;
	printf("\t\t   P1\tP2\tP3\tP4\tP5\tAverage\n");

	printf("Turnaround Time :  ");

	for (int i = 0; i < 5; i++)
	{
		printf("%d\t", tw[0][i]);	// turnaround time 출력
		turnaround_sum += tw[0][i];
	}

	printf("%.2f\n", turnaround_sum / 5);
	average[0] = turnaround_sum / 5;	// turnaround time 평균값 저장

	printf("   Waiting Time :  ");

	for (int i = 0; i < 5; i++)
	{
		printf("%d\t", tw[1][i]);	// waiting time 출력
		wait_sum += tw[1][i];
	}

	printf("%.2f\n", wait_sum / 5);
	average[1] = wait_sum / 5;	// waiting time 평균값 저장

	printf("\n");

	return 0;
}

int Print_Evaluation(double scheduling_aveTW[][6])
{
	printf("\t\t\tFCFS\t\tSJF\t\tPre_SJF\t\tPriority\tPre_Priority\tRR\n");

	printf("평균 Turnaround Time :\t");

	for (int i = 0; i < 6; i++)
		printf("%.2f\t\t", scheduling_aveTW[0][i]);	// 평균 Turnaround Time

	printf("\n");
	printf("   평균 Waiting Time : \t");

	for (int i = 0; i < 6; i++)
		printf("%.2f\t\t", scheduling_aveTW[1][i]);	// 평균 Waiting Time

	int turnaround_index = 0;
	double turnaround_min = scheduling_aveTW[0][0];	// 평균 Turnaround Time 최소값 저장, 가장 첫번째 값으로 초기화
	int waiting_index = 0;
	double waiting_min = scheduling_aveTW[1][0];	// 평균 Waiting Time 최소값 저장, 가장 첫번째 값으로 초기화

	for (int i = 1; i < 6; i++)	// i=0이 아닌 i=1(두번째)부터 시작
	{
		if (scheduling_aveTW[0][i] < turnaround_min)
		{
			turnaround_min = scheduling_aveTW[0][i];
			turnaround_index = i;
		}
		if (scheduling_aveTW[1][i] < waiting_min)
		{
			waiting_min = scheduling_aveTW[1][i];
			waiting_index = i;
		}
	}

	printf("\n\n평균 Turnaround Time이 가장 작은 스케줄링 기법 : %d\n", turnaround_index + 1);
	printf("   평균 Waiting Time이 가장 작은 스케줄링 기법 : %d\n", waiting_index + 1);

	return 0;
}


//-----------------------------------------------List queue node 구현-------------------------------------------------------------------------------------------
SNODE Create_SNODE()
{
	SNODE a;
	a.count = 0;
	a.next = NULL;
	return a;
}

int addLast(Process ps, SNODE *q)
{
	if (q->next == NULL)
	{
		q->count++;
		NODE *temp1 = (NODE*)malloc(sizeof(NODE));
		temp1->index = 0;
		temp1->next = NULL;
		temp1->ps = ps;
		q->next = (SNODE *)temp1;
		return 0;
	}
	else
	{
		q->count++;
		NODE *temp1 = (NODE *)q->next;
		int index = 0;

		while (temp1->next != NULL)
		{
			temp1 = temp1->next;
			index++;
		}

		NODE *temp2 = (NODE*)malloc(sizeof(NODE));
		temp2->ps = ps;
		temp2->index = index + 1;
		temp2->next = NULL;
		temp1->next = temp2;
		return 0;
	}
}


int addWant(Process ps, int index, SNODE *q)
{
	if (index < 0)
		return -1;

	else if (index == 0)
	{
		q->count++;
		NODE *temp1 = (NODE*)malloc(sizeof(NODE));	// temp1은 NODE만큼의 메모리공간을 가리킴, 즉 temp1은 addWant함수를 통해 들어온 한 프로세스를 가리킴
		temp1->ps = ps;
		temp1->index = index;
		temp1->next = (NODE *)q->next;	// ready queue가 비어있다면 temp1->next는 NULL을 가리킬것이고, ready queue에 어떤 프로세스가 대기하고있다면 이 프로세스를 가리키도록 함. 즉, ready queue에 차곡차곡 프로세를 링크하는 작업
		q->next = (SNODE *)temp1;
		temp1 = temp1->next;

		while (temp1 != NULL)
		{
			temp1->index++;
			temp1 = temp1->next;
		}

		return 0;
	}

	else
	{
		if (q->count - 1 < index)
			return -1;
		else
		{
			q->count++;
			NODE *temp1 = (NODE *)q->next;

			while (temp1->index != index - 1)
				temp1 = temp1->next;

			NODE *temp2 = (NODE*)malloc(sizeof(NODE));
			temp2->ps = ps;
			temp2->index = index;
			temp2->next = temp1->next;
			temp1->next = temp2;

			while (temp2->next != NULL)
			{
				temp2 = temp2->next;
				temp2->index++;
			}

			return 0;
		}
	}
}

Process getLast(SNODE *q)
{
	if (q->count == 0)	// ready queue가 비어있을 경우
		return Create_Null_Process();

	else if (q->count == 1) // ready queue안에 프로세스 한 개가 대기중임
	{
		q->count--;	// ready queue 안에있는 next가 한 메모리공간을 가리키고 있다는것은 이 메모리공간이 ready queue 안에서 대기하는 프로세스라고 생각하면됨
		NODE *a = (NODE *)q->next;	// a로하여금 현재 q가 가리키고있는 read queue에있는 next는 NODE형인 동적 메모리 공간을 가리키게함(이 안에는 ps,index,next가 들어있음) => 이 메모리 공간자체가 한개의 프로세스라 생각하면됨(ready queue에 적재되어있는), 즉 a는 ready queue 안에서 대기하는 프로세스 하나를 가리킴
		q->next = NULL; // 현재 q가 가리키고있는 read queue에있는 next가 NULL을 가리키게함으로써 ready queue와 프로세스간에 링크를 끊어버림(이 프로세스는 malloc()으로 할당한 메모리 공간
						// 즉 ready queue에 있던 프로세스를 run queue로 옮길 준비를 함
		return a->ps; // ready queue에서 대기하던 프로세스를 run queue(run구조체)로 반환함
					  // Ex) arrival == time에서 0으로 같은 값을 가질 때 arrival이 0인 프로세스 반환
	}
	else  // ready queue안에 프로세스 두개 이상이 대기중임 / Ex) FCFS에서는 "ready queue -> 두번째로 들어온 프로세스 -> 첫번째로 들어온 프로세스" (이런 식으로 노드가 연결되어있음) => ready queue안에 두개의 프로세스가 적재되어있는 상태
	{												   // Ex) SJF에서는 "ready queue -> 더 큰 cpuburst를 가진 프로세스 => 더 작은 cpuburst를 가진 프로세스"
		q->count--;
		NODE *a = (NODE *)q->next;
		NODE *b = a->next; // 
		while (b->next != NULL)
		{
			a = b;
			b = a->next;
		}
		a->next = NULL;	// FCFS에 경우 가장 먼저 ready queue에 적재된 프로세스를 return하기 위해 링크를 끊어버림

		return b->ps;	// FCFS에 경우 가장 먼저 ready queue에 적재된 프로세스 return~
	}
}

Process getWant(int index, SNODE *q)
{
	if (index <0)
		return Create_Null_Process();

	else if (index == 0)
	{
		q->count--;
		NODE *temp1 = (NODE *)q->next;
		NODE *temp2 = temp1->next;
		q->next = (SNODE *)temp2;

		while (temp2 != NULL)
		{
			temp2->index--;
			temp2 = temp2->next;
		}

		return temp1->ps;
	}

	else // index >= 1
	{
		if (q->count - 1 < index)
			return Create_Null_Process();
		else
		{
			q->count--;
			NODE *temp1 = (NODE *)q->next;
			NODE *temp2 = temp1->next;

			while (temp2->index != index)	// 찾고자 하는 index가 아닌 경우
			{
				temp1 = temp2;
				temp2 = temp1->next;
			}

			Process temp3 = temp2->ps;
			temp1->next = temp2->next;
			temp1 = temp1->next;

			while (temp1 != NULL)
			{
				temp1->index--;
				temp1 = temp1->next;
			}

			return temp3;
		}
	}
}
