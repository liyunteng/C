#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
/* #include <json/json.h> */

int main(int argc, char *argv[])
{
    json_object *jmsg;

    jmsg = json_object_new_object();
    json_object_object_add(jmsg, "module",
			   json_object_new_string("hello"));
    printf("%s\n", json_object_to_json_string(jmsg));
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test1 test1.c -g -ljson-c" */
/* End: */
