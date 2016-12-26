#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

struct People {
    char name[10];
    int age;
};

void p(int semid)
{
    struct sembuf sem_p;
    sem_p.sem_num = 0;
    sem_p.sem_op = -1;
    if (semop(semid, &sem_p, 1) == -1) {
	printf("p operation failed.\n");
    }
}

void v(int semid)
{
    struct sembuf sem_v;
    sem_v.sem_num = 0;
    sem_v.sem_op = 1;
    if (semop(semid, &sem_v, 1) == -1) {
	printf("v operation failed.\n");
    }
}


int main(int argc, char *argv[])
{
    int semid;
    int shmid;
    key_t semkey;
    key_t shmkey;

    semkey = ftok("server.c", 0);
    shmkey = ftok("client.c", 0);

    semid = semget(semkey, 0, 0666);
    if (semid == -1) {
	printf("create sem failed.\n");
	return -1;
    }
    shmid = shmget(shmkey, 0, 0666);
    if (shmid == -1) {
	printf("create shm failed.\n");
	return -1;
    }

    struct People *addr;
    addr = (struct People *) shmat(shmid, 0, 0);
    if (addr == (struct People *) -1) {
	printf("shm shmat failed.\n");
	return -1;
    }

    p(semid);
    printf("name:%s\n", addr->name);
    printf("age:%d\n", addr->age);
    v(semid);

    if (shmdt(addr) == -1) {
	printf("shmdt failed.\n");
    }

    if (semctl(semid, 0, IPC_RMID, 0) == -1) {
	printf("semctl delete error.\n");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
	printf("shmctl delete error.\n");
    }

    return 0;
}
