#include <stdio.h>
#include <string.h>
int
main(int argc, char *argv[])
{
    FILE *fp = NULL;
    char buf[4096];

    strcpy(buf, "mem is char dev!\n");
    printf("buf: %s\n", buf);

    fp = fopen("/dev/memdev0", "r+");
    if (fp == NULL) {
        printf("open memdev failed.\n");
        return -1;
    }

    fwrite(buf, sizeof(buf), 1, fp);
    fseek(fp, 0, SEEK_SET);
    strcpy(buf, "buf is null!");
    printf("buf: %s\n", buf);

    fread(buf, sizeof(buf), 1, fp);
    printf("buf: %s\n", buf);

    return 0;
}
