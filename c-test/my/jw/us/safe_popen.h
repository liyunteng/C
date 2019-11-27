#ifndef SAFE_POPEN__H__
#define SAFE_POPEN__H__

struct sp_child {
    pid_t pid;
    int fd;
};

int safe_popen(struct sp_child *c, const char *cmd);
int safe_system(const char *cmd);

#endif  // SAFE_POPEN__H__
