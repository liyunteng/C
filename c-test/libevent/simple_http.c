/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-01-14 16:44
* Filename : simple_http.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <event.h>
#include <evhttp.h>

void generic_request_handler(struct evhttp_request *req,
		void * arg)
{
	struct evbuffer *returnbuffer = evbuffer_new();

	evbuffer_add_printf(returnbuffer, "Thanks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", returnbuffer);
	evbuffer_free(returnbuffer);
	return ;
}

int main(int argc, char *argv[])
{
	short	http_port = 8081;
	char	*http_addr = "192.168.70.150";
	struct evhttp *http_server = NULL;

	event_init();
	http_server = evhttp_start(http_addr, http_port);
	evhttp_set_gencb(http_server, generic_request_handler, NULL);

	fprintf(stderr, "Server started on port %d\n", http_port);
	event_dispatch();

	return 0;

}
