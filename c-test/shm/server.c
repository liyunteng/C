#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

/* 信号量的p操作 */
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

struct People {
    char name[10];
    int age;
};

int main(int argc, char *argv[])
{

    int semid;
    int shmid;
    key_t semkey;
    key_t shmkey;

    semkey = ftok("server.c", 0);
    shmkey = ftok("client.c", 0);

    /* 创建共享内存和信号量的IPC */
    semid = semget(semkey, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        printf("create sem failed.\n");
        return -1;
    }
    shmid = shmget(shmkey, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        printf("create shm failed.\n");
        return -1;
    }

    /* 设置信号量的初始值 */
    union semum {
        int val;
        struct semid_ds *buf;
        ushort *array;
    } sem_u;

    sem_u.val = 1;
    semctl(semid, 0, SETVAL, sem_u);

    /* 将共享内存映射到当前进程的地址中，智慧直接对进程中的地址
     * addr操作就是对共享内存操作 */

    struct People *addr;
    addr = (struct People *) shmat(shmid, 0, 0);
    if (addr == (struct People *) -1) {
        printf("shm shmat failed.\n");
        return -1;
    }

    p(semid);
    strcpy((*addr).name, "xioaming");
    (*addr).age = 10;
    v(semid);

    /* 将共享内存与当前进程断开 */
    if (shmdt(addr) == -1) {
        printf("shmdt failed.\n");
        return -1;
    }
    return 0;
}
