#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>

#define SYSMON_ADDR "/root/soket/unix/test.domain"

int sysmon_event(const char *module,
		 const char *event, const char *param, const char *msg)
{
    struct sockaddr_un servaddr;
    socklen_t addr_len;
    size_t msg_len;
    int sockfd, ret;
    char msg_buf[1024];
    json_object *jmsg;

    if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
	return -1;
    }

    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SYSMON_ADDR);
    addr_len = strlen(servaddr.sun_path) + sizeof(servaddr.sun_family);

    jmsg = json_object_new_object();
    json_object_object_add(jmsg, "module", json_object_new_string(module));
    json_object_object_add(jmsg, "event", json_object_new_string(event));
    json_object_object_add(jmsg, "param", json_object_new_string(param));
    json_object_object_add(jmsg, "msg", json_object_new_string(msg));

    strcpy(msg_buf, json_object_to_json_string(jmsg));
    msg_len = strlen(msg_buf);

    /* ret = connect(sockfd, (struct sockaddr *)&servaddr, addr_len); */
    /* if (ret == -1){ */
    /*      printf("connect error!\n"); */
    /*      return -1; */
    /* } */

    /* send(sockfd, msg_buf, msg_len, 0); */
    sendto(sockfd, msg_buf, msg_len, 0, (struct sockaddr *) &servaddr,
	   addr_len);

    close(sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    while (1) {
	sysmon_event("abc", "def", "ghi", "adfaadjfkadljfakdjfadfad");
	sleep(1);
    }


}
