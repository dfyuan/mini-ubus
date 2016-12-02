#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>

#include "logd.h"
#include "sys_utils.h"
#include "signal_utils.h"
#include "event_mngr_utils.h"

static const char *git_version =
#include "netman_version.git"
;

int _log_v = LOG_LEVEL;

int service_quit = 0;

#define MAX_CLIENT           32
#define MAX_LISTEN_BACKLOG   6

#define MAX_TREAD_NAME_SIZE        32
#define MAX_EVENT_REGISTER_SIZE    16
#define MAX_EVENT_SUBSCRIBE_SIZE   16

struct register_thread_pool_t {
	int unix_socket;
	char thread_name[MAX_TREAD_NAME_SIZE];
	int event_register[MAX_EVENT_REGISTER_SIZE];
	int event_subscribe[MAX_EVENT_SUBSCRIBE_SIZE];
} register_thread_pool[32];

int idfi_cmd_handler(char *thread_name, int thread_pool_item_index)
{
	if (!strcmp(thread_name, register_thread_pool[thread_pool_item_index].thread_name)) {
		MSG("Thread (%s) have been registed\n", thread_name);
		return -1;
	}

	strcpy(register_thread_pool[thread_pool_item_index].thread_name, thread_name);
	MSG("Thread (%s) is registed\n", register_thread_pool[thread_pool_item_index].thread_name);
	return 0;
}

int reg_cmd_handler(struct string_array event_msg, int thread_pool_item_index)
{
	MSG("stub func api\n");
	return 0;
}

int sub_cmd_handler(int event_id, int thread_pool_item_index)
{
	int i;

	if (!strcmp(register_thread_pool[thread_pool_item_index].thread_name, "NULL")) {
		MSG("Unregisted thread's cmd, just abandon it\n");
		return -1;
	}

	for (i = 0; i < MAX_EVENT_SUBSCRIBE_SIZE; i++) {
		if (register_thread_pool[thread_pool_item_index].event_subscribe[i] == event_id) {
			MSG("Thread(%s) have subscribed event(%d), don't re-subscribed\n", register_thread_pool[thread_pool_item_index].thread_name, event_id);
			return -1;
		}

		if (register_thread_pool[thread_pool_item_index].event_subscribe[i] == 0) {
			register_thread_pool[thread_pool_item_index].event_subscribe[i] = event_id;
			MSG("Thread(%s) subscribe event(%d) index(%d)\n", register_thread_pool[thread_pool_item_index].thread_name,
			       register_thread_pool[thread_pool_item_index].event_subscribe[i], i);
			return 0;
		}
	}

	WARN("Thread(%s) there no space for event(%d)\n", register_thread_pool[thread_pool_item_index].thread_name, event_id);
	return -1;
}

int pubint_cmd_handler(int event_id, int event_value, int thread_pool_item_index)
{
	int i;
	int j;
	char send_buf[64];

	sprintf(send_buf, "intent:%d:%d\n\r", event_id, event_value);

	for (i = 0; i < sizeof(register_thread_pool) / sizeof(struct register_thread_pool_t); i++) {
		if (strcmp(register_thread_pool[thread_pool_item_index].thread_name, register_thread_pool[i].thread_name) &&
					strcmp("NULL", register_thread_pool[i].thread_name)) {
			for (j = 0; j < MAX_EVENT_SUBSCRIBE_SIZE; j++) {
				if (register_thread_pool[i].event_subscribe[j] == event_id) {
					DBG("pub to thread(%s) : event_id(%d) : event_value(%d)\n", register_thread_pool[i].thread_name, event_id, event_value);
					write(register_thread_pool[i].unix_socket, send_buf, strlen(send_buf));
				}
			}
		}
	}

	return 0;
}

int pubbuf_cmd_handler(struct event_packet_s *p_event_packet, int thread_pool_item_index)
{
	int i;
	int j;

	int event_id = p_event_packet->event_id;

	for (i = 0; i < sizeof(register_thread_pool) / sizeof(struct register_thread_pool_t); i++) {
		if (strcmp(register_thread_pool[thread_pool_item_index].thread_name, register_thread_pool[i].thread_name) &&
					strcmp("NULL", register_thread_pool[i].thread_name)) {
			for (j = 0; j < MAX_EVENT_SUBSCRIBE_SIZE; j++) {
				if (register_thread_pool[i].event_subscribe[j] == event_id) {
					DBG("pubbuf to thread(%s) : event_id(%d)\n", register_thread_pool[i].thread_name, event_id);
					write(register_thread_pool[i].unix_socket, p_event_packet->raw_data, p_event_packet->data_len+EVENT_PACKET_HEAD_LEN);
				}
			}
		}
	}

	return 0;
}

#if 0
struct bs_handler_t {
	char *cmd_type;
	int (*cmd_handler)(struct string_array event_msg, int thread_pool_item_index);
} bs_handlers[] = {
	{ "idfi", idfi_cmd_handler },
	{ "reg", reg_cmd_handler },
	{ "pub", pub_cmd_handler },
	{ "pubstr", pubstr_cmd_handler },
	{ "sub", sub_cmd_handler },
};


int bs_man_handler(struct string_array event_msg, int thread_pool_item_index)
{
	int i;

	for (i = 0; i <  sizeof(bs_handlers) / sizeof(struct bs_handler_t); i++) {
		if (!strcmp(event_msg.string[0], bs_handlers[i].cmd_type)) {
			//DBG("CALL CMD (%s)\n", bs_handlers[i].cmd_type);
			bs_handlers[i].cmd_handler(event_msg, thread_pool_item_index);
			break;
		}
	}

	return 0;
}
#endif
static void reset_register_thread_pool_item(int item_index)
{
	register_thread_pool[item_index].unix_socket = -1;
	memset(&register_thread_pool[item_index], 0, sizeof(struct register_thread_pool_t));
	strcpy(register_thread_pool[item_index].thread_name, "NULL");
}

int event_packet_handler(struct event_packet_s *p_event_packet, int thread_pool_item_index)
{
	switch (p_event_packet->event_type) {
		case EVENT_TYPE_IDFI:
			idfi_cmd_handler((char *)p_event_packet->data, thread_pool_item_index);
			break;

		case EVENT_TYPE_SUB:
			sub_cmd_handler(p_event_packet->event_id, thread_pool_item_index);
			break;

		case EVENT_TYPE_PUBINT:
			pubint_cmd_handler(p_event_packet->event_id, *(int *)(p_event_packet->data), thread_pool_item_index);
			break;

		case EVENT_TYPE_PUBBUF:
			pubbuf_cmd_handler(p_event_packet, thread_pool_item_index);
			break;

		default :
			WARN("Invalid event type\n");
			break;
	}

	return 0;
}

void *event_mngr_thread_func(void * arg)
{
	int i;
	int ret;
	int em_unix_sock;
	fd_set rfds;
	int max_fd;
	int offset;
	struct timeval tv;
	struct sockaddr_un sun;

	struct event_packet_s event_packet;
	int recv_len;

	em_unix_sock = socket(AF_LOCAL, SOCK_SEQPACKET, 0);
	if (em_unix_sock < 0) {
		ERR("Create bs unix sock error\n");
		goto err_out;
	}

	fcntl(em_unix_sock, F_SETFD, fcntl(em_unix_sock, F_GETFD, 0) | FD_CLOEXEC);

	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, EVENT_MNGR_UNIX_SOCK);
	unlink(sun.sun_path);

	offset = offsetof(struct sockaddr_un, sun_path) + strlen(sun.sun_path);
	ret = bind(em_unix_sock, (const struct sockaddr*)&sun, (socklen_t)offset);
	if (ret) {
		ERR("Bind bs unix sock error\n");
		goto err_out;
	}

	ret = listen(em_unix_sock, MAX_LISTEN_BACKLOG);
	if (ret) {
		ERR("Listen bs unix sock error\n");
		goto err_out;
	}

	MSG("Bs man service listen on socket: %s\n", EVENT_MNGR_UNIX_SOCK);

	memset(register_thread_pool, 0, sizeof(register_thread_pool));
	for (i = 0; i < sizeof(register_thread_pool) / sizeof(struct register_thread_pool_t); i++) {
		strcpy(register_thread_pool[i].thread_name, "NULL");
	}

	while (!service_quit) {
		FD_ZERO(&rfds);
		FD_SET(em_unix_sock, &rfds);
		/* event driven: loop every 1s is ok */
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		max_fd = em_unix_sock;

		for (i = 0; i < MAX_CLIENT; i++) {
			if (register_thread_pool[i].unix_socket <= 0) {
				continue;
			}

			FD_SET(register_thread_pool[i].unix_socket, &rfds);
			if (register_thread_pool[i].unix_socket > max_fd) {
				max_fd = register_thread_pool[i].unix_socket;
			}
		}

		//char *p = NULL;
		ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret <= 0) {
			/* Need find the root casue: why this usleep(100*1000) will cause process take more than 1% cpu */
			//usleep(100*1000);
			continue;
		}

		/*lisening socket*/
		if (FD_ISSET(em_unix_sock, &rfds)) {
			for (i = 0; i < MAX_CLIENT; i++) {
				if (register_thread_pool[i].unix_socket <= 0)
					break;
			}

			if (i < MAX_CLIENT) {
				socklen_t slen = sizeof(sun);
				register_thread_pool[i].unix_socket = accept(em_unix_sock, (struct sockaddr*)&sun, &slen);
				if (register_thread_pool[i].unix_socket > 0) {
					slen -= offsetof(struct sockaddr_un, sun_path);
					sun.sun_path[slen] = 0;

					MSG("New client[%d]: %s, %d\n", i, sun.sun_path, register_thread_pool[i].unix_socket);

					ret = fcntl(register_thread_pool[i].unix_socket, F_SETFL,
					            fcntl(register_thread_pool[i].unix_socket, F_GETFL, 0) | O_NONBLOCK);
				} else {
					ERR("Broadcast service accept error\n");
				}
			}
		}

		for (i = 0; i < MAX_CLIENT; i++) {
			/* make sure register_thread_pool[i].unix_socket > 0 */
			if (register_thread_pool[i].unix_socket <= 0) {
				continue;
			}

			if (!FD_ISSET(register_thread_pool[i].unix_socket, &rfds)) {
				continue;
			}

			memset(&event_packet, 0, sizeof(event_packet));

			recv_len = recv(register_thread_pool[i].unix_socket, event_packet.raw_data, sizeof(event_packet.raw_data) - 1, 0);
			if (recv_len <= 0) {
				if (recv_len < 0) {
					WARN("Recv client package error");
				} else {
					WARN("Connection closed by peer!\n");
				}

				MSG("Close client[%d]\n", i);
				close(register_thread_pool[i].unix_socket);
				reset_register_thread_pool_item(i);
				continue;
			}

			if (parse_event_packet(&event_packet, recv_len) < 0) {
				WARN("Parse event packet err\n");
				continue;
			}

			event_packet_dump(&event_packet);
			event_packet_handler(&event_packet, i);
		}
	}

	MSG("Broadcast manager services quit!\n");

err_out:
	for (i = 0; i < MAX_CLIENT; i++) {
		if (register_thread_pool[i].unix_socket > 0)
			close(register_thread_pool[i].unix_socket);
	}
	if (em_unix_sock > 0)
		close(em_unix_sock);

	return 0;
}

int main()
{
	logd_init("/tmp/event_mngr.log");
	BUILD_INFO_LOG();
	signal_setup();
	event_mngr_thread_func(NULL);
	logd_deinit();
	return 0;
}
