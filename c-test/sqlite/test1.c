#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

int
print_record(void *params, int n_column, char **column_value, char **column_name)
{
    int i;
    for (i = 0; i < n_column; i++) {
        printf("\t%s", column_value[i]);
    }
    printf("\n");
    return 0;
}

int
main(int argc, char *argv[])
{
    const char *  sql_create_table = "create table t(id int primary key, msg varchar(128));";
    char *        errmsg, **result;
    int           ret = 0;
    int           col, row, i, j;
    sqlite3 *     db = NULL;
    sqlite3_stmt *stmt;
    char          cmd[256];

    ret = sqlite3_open_v2("./test.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        return ret;
    }
    printf("Open database\n");

    // ret = sqlite3_exec(db, "drop table if exists t", NULL, NULL, &errmsg);
    // if (ret != SQLITE_OK) {
    //      goto quit;
    //}

    ret = sqlite3_exec(db, sql_create_table, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "create table error:%s\n", errmsg);
        goto quit;
    }
    ret = sqlite3_exec(db, "insert into t(id, msg) values(0, 'test0')", NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
        goto quit;

    sqlite3_prepare_v2(db, "insert into t(id, msg) values(?, ?)", -1, &stmt, 0);
    for (i = 1; i < 20; i++) {
        sprintf(cmd, "test%d", i);
        sqlite3_bind_int(stmt, 1, i);
        sqlite3_bind_text(stmt, 2, cmd, strlen(cmd), NULL);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);

    ret = sqlite3_exec(db, "select * from t", print_record, NULL, &errmsg);
    if (ret != SQLITE_OK)
        goto quit;

    ret = sqlite3_get_table(db, "select * from t;", &result, &row, &col, &errmsg);
    if (ret != SQLITE_OK)
        goto quit;

    printf("col:%d  row:%d\n", col, row);
    for (i = 0; i <= row; i++) {
        for (j = 0; j < col; j++) {
            printf("%s\t", result[i * col + j]);
        }
        printf("\n---------------------------\n");
    }

quit:
    sqlite3_free(errmsg);
    sqlite3_free_table(result);
    sqlite3_close(db);

    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test1 test1.c -g -lsqlite3" */
/* End: */
