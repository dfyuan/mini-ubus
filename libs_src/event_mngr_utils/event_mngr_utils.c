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

#include "event_mngr_utils.h"
#include "sys_utils.h"
#include "logd.h"

static int get_frame_stx(unsigned char *buf)
{
	int *p;

	p = (int *)buf;
	return *(p + 0);
}

static int get_frame_event_type(unsigned char *buf)
{
	int *p;

	p = (int *)buf;
	return *(p + 1);
}

static int get_frame_event_id(unsigned char *buf)
{
	int *p;

	p = (int *)buf;
	return *(p + 2);
}

static int get_frame_data_len(unsigned char *buf)
{
	int *p;

	p = (int *)buf;
	return *(p + 3);
}

int parse_event_packet(struct event_packet_s *p_event_packet, int len)
{
	unsigned char *buf = p_event_packet->raw_data;

	if (get_frame_stx(p_event_packet->raw_data) != EVENT_PACKET_HEAD) {
		ERR("Err event packet stx (%d)\n", get_frame_stx(buf));
		return -1;
	}

	if (len != (get_frame_data_len(buf)+EVENT_PACKET_HEAD_LEN)) {
		ERR("Err event packet data len(%d:%d)\n", len, get_frame_data_len(buf));
		return -1;
	}

	p_event_packet->event_type = get_frame_event_type(buf);
	p_event_packet->event_id = get_frame_event_id(buf);
	p_event_packet->data_len = get_frame_data_len(buf);

	p_event_packet->data = p_event_packet->raw_data + EVENT_PACKET_HEAD_LEN;

	return 0;
}

int event_packet_dump(struct event_packet_s * p_event_packet)
{
	int i;
	return 0;

	printf("\n####################################################\n");
	printf("event type : %d\n", p_event_packet->event_type);
	printf("event id   : %d\n", p_event_packet->event_id);
	printf("data_len   : %d\n", p_event_packet->data_len);

	printf("data : \n");
	if (p_event_packet->event_type == EVENT_TYPE_IDFI) {
		printf("%s\n", p_event_packet->data);
	} else {
		for (i = 0; i < p_event_packet->data_len; i++) {
			if ((i+1)%32 == 0) {
				printf("\n");
			}
			printf("0x%02x ", p_event_packet->data[i]);
		}
		printf("\n");
	}
	printf("####################################################\n\n");
}

static void event_packet_to_frame(struct event_packet_s *p_event_packet, char *data)
{
	int *p;

	p = (int *)p_event_packet->raw_data;
	*(p + 0) = EVENT_PACKET_HEAD;
	*(p + 1) = p_event_packet->event_type;
	*(p + 2) = p_event_packet->event_id;
	*(p + 3) = p_event_packet->data_len;

	memcpy(p_event_packet->raw_data + EVENT_PACKET_HEAD_LEN, data, p_event_packet->data_len);
}

static void frame_to_event_packet(struct event_packet_s *p_event_packet, char *data)
{
	int *p;

	p = (int *)p_event_packet->raw_data;
	*(p + 0) = EVENT_PACKET_HEAD;
	*(p + 1) = p_event_packet->event_type;
	*(p + 2) = p_event_packet->event_id;
	*(p + 3) = p_event_packet->data_len;

	memcpy(p_event_packet->raw_data + EVENT_PACKET_HEAD_LEN, data, p_event_packet->data_len);
}

static void event_packet_splice(struct event_packet_s *p_event_packet, int event_type, int event_id, char *data, int data_len)
{
	p_event_packet->event_type = event_type;
	p_event_packet->event_id = event_id;
	p_event_packet->data_len = data_len;
	event_packet_to_frame(p_event_packet, data);
}

/*
 * register thread name to broadcast manager service
 * idfi(identifier):thread_name
 */
int register_thread_name(int unix_socket, char *thread_name)
{
	printf("thread_name = %s\n", thread_name);
	struct event_packet_s event_packet;
	event_packet_splice(&event_packet, EVENT_TYPE_IDFI, -1, thread_name, strlen(thread_name)+1);
	write(unix_socket, event_packet.raw_data, event_packet.data_len + EVENT_PACKET_HEAD_LEN);

	return 0;
}

/*
 * register event it provided to broadcast manager service
 * reg:event_id
 */
int register_event(int unix_socket, int event_id)
{
	struct event_packet_s event_packet;
	event_packet_splice(&event_packet, EVENT_TYPE_REG, event_id, NULL, 0);
	write(unix_socket, event_packet.raw_data, event_packet.data_len + EVENT_PACKET_HEAD_LEN);

	return 0;
}

/*
 * subscribe event it provided to broadcast manager service
 * reg:event_id
 */
int subscribe_event(int unix_socket, int event_id)
{
	struct event_packet_s event_packet;
	event_packet_splice(&event_packet, EVENT_TYPE_SUB, event_id, NULL, 0);
	write(unix_socket, event_packet.raw_data, event_packet.data_len + EVENT_PACKET_HEAD_LEN);

	return 0;
}

/*
 * publish event (int) to broadcast manager service
 * pub:event_id:event_value
 */
int publish_event(int unix_socket, int event_id, int event_state)
{
	struct event_packet_s event_packet;
	event_packet_splice(&event_packet, EVENT_TYPE_PUBINT, event_id, (char *)&event_state, sizeof(int));
	write(unix_socket, event_packet.raw_data, event_packet.data_len + EVENT_PACKET_HEAD_LEN);

	return 0;
}

/*
 * publish event buf to broadcast manager service
 * pub:event_id:event_value
 */
int publish_event_buf(int unix_socket, int event_id, char *buf, int len)
{
	struct event_packet_s event_packet;
	event_packet_splice(&event_packet, EVENT_TYPE_PUBBUF, event_id, buf, len);
	write(unix_socket, event_packet.raw_data, event_packet.data_len + EVENT_PACKET_HEAD_LEN);

	return 0;
}

int connect_event_mngr()
{
	int ret = -1;
	int retry_time = 0;
	int sock;
	struct sockaddr_un address;
	size_t addrLength;

	if ((sock = socket(AF_LOCAL, SOCK_SEQPACKET, 0)) < 0) {
		printf("Create socket err\n");
		return -1;
	}

	address.sun_family = AF_UNIX;	/* Unix domain socket */
	strcpy(address.sun_path, EVENT_MNGR_UNIX_SOCK);

	/* The total length of the address includes the sun_family element */
	addrLength = sizeof(address.sun_family) + strlen(address.sun_path);

	ret = connect(sock, (struct sockaddr *) &address, addrLength);
	while (ret < 0) {
		if (retry_time++ > 10) {
			break;
		}
		printf("re-connect...\n");
		usleep(500*1000);
		ret = connect(sock, (struct sockaddr *) &address, addrLength);
	}

	if (ret != 0) {
		printf("Connect broadcast server err\n");
		close(sock);
		return -1;
	}

	return sock;
}

#if 1
int parse_broadcast_event(char *buf, struct br_event_t *br_event)
{
	struct string_array event_msg;

	os_strsep(buf, &event_msg, ":");

	br_event->event_id = atoi(event_msg.string[1]);
	br_event->event_state = atoi(event_msg.string[2]);

	return 0;
}

int parse_broadcast_event_string(char *buf, struct br_event_string_t *br_event)
{
	struct string_array event_msg;

	os_strsep(buf, &event_msg, ":");

	memset(&event_msg, 0, sizeof(struct string_array));
	br_event->event_id = atoi(event_msg.string[1]);
	strcpy(br_event->event_string, event_msg.string[2]);

	return 0;
}
#endif
