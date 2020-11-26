#include <json-c/json.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <json/json.h> */

int n = 0;
int
create(json_object *obj, json_object *arr, char *name, int age, char *sex)
{
    obj = json_object_new_object();
    json_object_object_add(obj, "name", json_object_new_string(name));
    json_object_object_add(obj, "age", json_object_new_int(age));
    json_object_object_add(obj, "sex", json_object_new_string(sex));
    json_object_array_add(arr, obj);
    n++;
    return 0;
}

int
main(void)
{
    int i;
    json_object *root = NULL, *new = NULL, *tmp = NULL;
    setlocale(LC_ALL, "");

    root = json_object_new_object();
    new  = json_object_new_array();

    json_object_object_add(root, "arr", new);
    create(tmp, new, "lyt", 30, "男");
    create(tmp, new, "aaa", 20, "nv");
    create(tmp, new, "bbb", 10, "女");
    create(tmp, new, "ccc", 10, "男");
    json_object_object_add(root, "totle", json_object_new_int(n));
    printf("%s\n", json_object_to_json_string(root));

    json_object_array_put_idx(new, 1, NULL);
    printf("%s\n", json_object_to_json_string(root));

    printf("%s\n",
           json_object_to_json_string(json_object_array_get_idx(new, 0)));

    json_object_object_foreach(root, key, val)
    {
        printf("%s:%s\n", key, json_object_to_json_string(val));
    }

    for (i = 0; i < json_object_array_length(new); i++) {
        printf("%s\n",
               json_object_to_json_string(json_object_array_get_idx(new, i)));
    }
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test4 test4.c -g -ljson-c" */
/* End: */
