/* gcc -o json_test2 json_test2.c -ljson-c */

#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

int json_verify_get_int(char *string, char *key, int *value)
{
	struct json_object *new_obj, *tmp_obj;

	new_obj = json_tokener_parse(string);
	if (new_obj == NULL) {
		printf("%s: Not in json format: %s\n", __func__, string);
		json_object_put(new_obj);
		return -1;
	}

	if (!json_object_object_get_ex(new_obj, key, &tmp_obj)) {
		json_object_put(new_obj);
		return -1;
	}

	if (!json_object_is_type(tmp_obj, json_type_int)) {
		json_object_put(new_obj);
		return -1;
	}


	*value = json_object_get_int(tmp_obj);

	json_object_put(new_obj);
	return 0;
}


int main(int argc, char **argv)
{
	char *string = "{\"method\":\"_internal.res_wifi_conf_status\",\"params\":1}";
	int status = -1;

	printf("Parse string: %s\n", string);
	json_verify_get_int(string, "params", &status);
	printf("status: %d\n", status);
}
