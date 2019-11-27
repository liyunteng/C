#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BlockSize 512
#define DirSize 32
#define RootSize 2

struct ReserveBlock {
    int sysblocknum;                            /* 文件系统总扇区数 */
    int resblocknum;                            /* 保留扇区扇区数 */
    int fatblocknum;                            /* fat表扇区数 */
    int rootblocknum;                           /* 根目录山区数 */
    char fillchar[BlockSize - 4 * sizeof(int)]; /* 填充字节 */
};

struct DirBlock {
    char filename[11]; /* 文件名长度限制 */
    char fillchar[DirSize - 4 * sizeof(int) - sizeof(long int) - 11]; /* 填
                                                                       * 充
                                                                       * 字
                                                                       * 节
                                                                       *  */
    long filelen;         /* 文件长度 */
    int year, month, day; /* 日期 */
    int firstblockaddr;   /* 文件首块扇区号 */
};

struct FCBBlock {             /* 文件控制块 */
    int fileid;               /* 文件标志 */
    struct DirBlock fileinfo; /*  目录信息 */
    long filpos;              /* 文件读写指针 */
    int fdtblockaddr;         /* 目录项所在块号 */
    int fdtblockindex;        /* 目录项所在块内序号 */
    struct FCBBlock *next;    /* 下个文件控制块 */
};

struct ReserveBlock sys1;
struct FCBBlock *fcb;
struct DirBlock fil[32], *dir; /* 目录项 */
int *fat1;
char *str, *ptr;
char fillchar[BlockSize];
FILE *fp;

FILE *
OPENSYS(char *filename)
{
    int i;
    fp = fopen(filename, "rb+");
    printf("%d %d %d %d\n", sys1.sysblocknum, sys1.resblocknum,
           sys1.fatblocknum, sys1.rootblocknum);
    fat1 = (int *)malloc(sys1.sysblocknum);
    for (i = 0; i < sys1.fatblocknum; i++) {
        fread(fat1, sizeof(int) * sys1.sysblocknum, 1, fp);
        /* 读入基本的文件系统 */
    }
    fseek(fp, (sys1.fatblocknum + sys1.resblocknum) * BlockSize, 0);
    /* 修改文件指针位置 */
    dir = fil; /* 目录指针 */
    fread(dir, DirSize * 32, 1, fp);
    return fp;
}

int
CLOSESYS(FILE *stream)
{
    int i;
    fseek(stream, sys1.resblocknum * BlockSize, 0);
    fwrite(fat1, sizeof(int) * sys1.sysblocknum, i, stream);
    fseek(fp, (sys1.fatblocknum + sys1.resblocknum) * BlockSize, 0);
    fwrite(dir, DirSize * 32, 1, fp);
    fclose(fp);
    return 1;
}

void
LISTDIR(void)
{
    int i, flag = 0;
    for (i = 0; i < 32; i++) {
        if (fil[i].firstblockaddr != 0) {
            if (flag == 0)
                printf("filename  file_creat_time:\n");
            flag = 1;
            printf("%-15s  %d/%d/%d \n", fil[i].filename, fil[i].year,
                   fil[i].month, fil[i].day);
        }
    }
}

int
FCREATE(char *filename)
{
    int a, i, j, flag1 = 0, flag2 = 0, k = 0;
    int n, m;

    while (1) {
        a = strlen(filename);
        if (a > 10) {
            printf("the length of file's name is too long!\n");
            printf("input file's name again:");
            scanf("%s", filename);
        } else {
            break;
        }
        while (1) {
            if (strcmp(filename, fil[i].filename) == 0) {
                printf("the name already exist\n");
                printf("input name again:");
                flag1 = 1;
                break;
            }
            if (flag = 0)
                break;
            scanf("%s", filename);
            flag1 = 0;
        }

        for (i = (sys1.fatblocknum + sys1.resblocknum + sys1.rootblocknum);
             i < sys1.sysblocknum; i++) {
            if (fat[i] == 0)
                flag++; /* 统计磁盘上为空数目 */
            if (flag == 0) {
                printf("disk is full\n");
                return 0;
            }
            printf("number of free block: %d\n", flag);
            printf("input file length:");
            for (j = 0; j < 32; j++) {
                if (fil[j].firstblockaddr == 0)
                    break;
            }
            while (1) {
                scanf("%d", &dir[j].filelen);
                n = (dir[j].filelen / BlockSize)
                    + (dir[j].filelen / BlockSize ? 1 : 0);
                if (n < 0 || n > flag) {
                    printf("input length too long");
                    printf("input again:");
                } else {
                    break;
                }
            }

            for (i = (sys1.fatblocknum + sys1.resblocknum + sys1.rootblocknum);
                 i < sys1.sysblocknum; i++) {
                if (fat1[i] == 0) {
                    k++;
                    if (flag2 == 0) {
                        dir[j].firstblockaddr = i + 1;
                        flag2                 = 1;
                    } else {
                        if (k >= n) {
                            fat1[m] = i + 1;
                            fat1[i] = -1;
                            break;
                        } else {
                            fat1[m] = i + 1;
                        }
                    }
                    m = i;
                }
            }
        }
    }
    strcpy(dir[j].filename, filename);
    dir[j].filename[a] = '\0';
    printf("input year:");
    scanf("%d", % dir[j].year);
    printf("input month:");
    scanf("%d", &dir[j].mounth);
    printf("input day:");
    scanf("%d", &dir[j].day);
    return 1
}

int
FDELETE(char *filenmae)
{
}
