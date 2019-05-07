/* gcc -o json_test json_test.c -ljson-c */

#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s string\n", argv[0]);
		exit(-1);
	}

	char *str = argv[1];
	struct json_tokener *tokener;
	struct json_object *json;
	int totallen, len;

	tokener = json_tokener_new();
	totallen = strlen(str);
	len = totallen;

	printf("try to parse: %s\n", str);
	while (len > 0) {
		json = json_tokener_parse_ex(tokener, str, len);
		if (json == NULL) {
			printf("parse error: %s.\n", json_tokener_error_desc(json_tokener_get_error(tokener)));
			goto out;
		}

		printf("extracted %s\n", json_object_to_json_string(json));
		len -= tokener->char_offset;
		str += tokener->char_offset;
	}

out:
	json_tokener_free(tokener);
	json_object_put(json);
}
