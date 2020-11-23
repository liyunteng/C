#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define REG "pci0000:00/0000:00:1c.[0-9]+.*/ata([0-9]+)/.*/block/sd[a-z]"
static char *
outstr(const char *str, unsigned start, unsigned end)
{
    unsigned n = end - start;
    static char stbuf[256];
    strncpy(stbuf, str + start, n);
    stbuf[n] = 0;
    return stbuf;
}

int
main(int argc, char *argv[])
{
    int x, z, lno = 0;
    char ebuf[128], lbuf[256];
    regex_t reg;
    regmatch_t pm[10];
    const size_t nmatch = 10;
    FILE *fp;

    z = regcomp(&reg, REG, REG_EXTENDED);
    if (z != 0) {
        regerror(z, &reg, ebuf, sizeof(ebuf));
        fprintf(stderr, "%s: pattern '%s'\n", ebuf, REG);
        return -1;
    }
    fp = fopen("./regex_data1", "r");
    if (!fp) {
        fprintf(stderr, "fopen error!\n");
        return -1;
    }

    while (fgets(lbuf, sizeof(lbuf), fp)) {
        ++lno;

        if ((z = strlen(lbuf)) > 0 && lbuf[z - 1] == '\n')
            lbuf[z - 1] = 0;
        z = regexec(&reg, lbuf, nmatch, pm, 0);
        if (z == REG_NOMATCH)
            continue;
        else {

            printf(" %d : %s\n", lno, lbuf);
            for (x = 0; x < nmatch && pm[x].rm_so != -1; x++) {
                printf("  $%d='%s'\n", x,
                       outstr(lbuf, pm[x].rm_so, pm[x].rm_eo));
            }
        }
    }
    regfree(&reg);
    return 0;
}
