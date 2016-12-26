#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <json/json.h>

int main(int argc, char *argv[])
{
	json_object *root,  *value;
	setlocale(LC_ALL, "");

	root = json_object_new_object();
	json_object_object_add(root, "name", json_object_new_string("lyt"));
	json_object_object_add(root, "age", json_object_new_int(30));
	json_object_object_add(root, "sex", json_object_new_string("男"));

	printf("%s\n",json_object_to_json_string(root));

	return 0;
}
