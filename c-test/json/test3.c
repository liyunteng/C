#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <json-c/json.h>
/* #include <json/json.h> */

int main(int argc, char *argv[])
{
    json_object *root, *new;
    setlocale(LC_ALL, "");

    root = json_object_new_object();
    json_object_object_add(root, "name", json_object_new_string("lyt"));
    json_object_object_add(root, "age", json_object_new_int(30));
    json_object_object_add(root, "sex", json_object_new_string("男"));

    new = json_object_new_array();
    json_object_array_add(new, json_object_new_int(3));
    json_object_array_add(new, json_object_new_int(10));
    json_object_array_add(new, json_object_new_int(2));

    json_object_object_add(root, "arr", new);

    printf("%s\n", json_object_to_json_string(root));

    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test3 test3.c -g -ljson-c" */
/* End: */
