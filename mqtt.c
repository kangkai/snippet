#include <stdio.h>
#include <string.h>
#include <mosquitto.h>

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if(message->payloadlen){
		printf("%s %s\n", message->topic, (char *)message->payload);
	}else{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result){
		/* Subscribe to broker information topics on successful connect. */
		mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
		printf("connected.\n");
	}else{
		fprintf(stderr, "Connect failed\n");
	}
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;

	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	printf("%s\n", str);
}

int main(int argc, char *argv[])
{
	int i;
	char *host = "localhost";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;
	char buf[80];
	int count;
	struct mosquitto *mosq = NULL;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	mosquitto_log_callback_set(mosq, my_log_callback);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

	if(mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		return 1;
	}

	printf("before loop.\n");
	i = mosquitto_publish(mosq, NULL, "test_publish", 6, "hello1", 0, false);
	if (i)
		printf("publish ret: %d\n", i);
	sleep(1);
	i = mosquitto_publish(mosq, NULL, "test_publish", 6, "hello2", 0, false);
	if (i)
		printf("publish ret: %d\n", i);
	sleep(10);

	printf("Enter loop.\n");
	count = 3;
	while(!mosquitto_loop(mosq, -1, 1)){
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "hello%d", count);
		i = mosquitto_publish(mosq, NULL, "test_publish", strlen(buf), buf, 0, false);
		if (i)
			printf("publish ret: %d\n", i);

		sleep(1);
		count++;
	}
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}
	
