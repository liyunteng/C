#ifndef SVMSG__H__
#define SVMSG__H__

#define MAXMSG 16384

#define MQ_KEY1 1234L
#define MQ_KEY2 2345L

struct msgbuf {
    long mtype;
    char mtext[0];
};

struct mymsg {
    long len;
    long mtype;
    char data[MAXMSG];
};

#endif  // SVMSG__H__
