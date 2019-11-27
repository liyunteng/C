#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define REG_MV_DISK_SLOT                                                       \
    "pci0000:00/0000:00:1c.[0-9]+.*/ata([0-9])+/.*/block/sd[a-z]"

int
main(int argc, char *argv[])
{
    FILE *fp;
    regex_t mv_disk_slot_regex, test_regex;
    regmatch_t pmatch[10];
    int ret;
    char sbuf[200];
    char buf[1024];
    int i, j;

    ret = regcomp(&mv_disk_slot_regex, REG_MV_DISK_SLOT, REG_EXTENDED);
    if (ret != 0) {
        fprintf(stderr, "regcomp error!\n");
        return -1;
    }

    if ((fp = fopen("./out", "r")) == NULL) {
        fprintf(stderr, "fopen error!\n");
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        ret = regexec(&mv_disk_slot_regex, buf, 10, pmatch, 0);
        if (ret == REG_NOMATCH)
            continue;
        else {
            for (j = 0; j < 10; j++) {
                i = pmatch[j].rm_eo - pmatch[j].rm_so;
                strncpy(sbuf, &buf[pmatch[j].rm_so], i);
                sbuf[i] = '\0';
                printf("start:%d end:%d i:%d\t%s\n", pmatch[j].rm_so,
                       pmatch[j].rm_eo, i, sbuf);
            }
            printf("====================================\n");
        }
    }

    regfree(&mv_disk_slot_regex);
    return 0;
}
