#ifndef EVENT_MNGR_UTILS_H_
#define EVENT_MNGR_UTILS_H_

#define MAX_PACKET_BUF		8*1024
#define EVENT_PACKET_HEAD	0xdeadbeef

#define EVENT_TYPE_IDFI		1
#define EVENT_TYPE_REG		2
#define EVENT_TYPE_SUB		3

#define EVENT_TYPE_PUBINT	11
#define EVENT_TYPE_PUBBUF	12
#define EVENT_TYPE_PUBSTR	13

#define EVENT_PACKET_HEAD_LEN	(sizeof(int)*4)
#define EVENT_PACKET_DATA_OFFSET	EVENT_PACKET_HEAD_LEN

#define EVENT_MNGR_UNIX_SOCK  "/tmp/event_mngr"

struct event_packet_s {
	unsigned int event_type;
	unsigned int event_id;
	unsigned int data_len;
	unsigned char *data;
	unsigned char raw_data[MAX_PACKET_BUF];
};

struct br_event_t {
	int event_id;
	int event_state;
};

struct br_event_string_t {
	int event_id;
	char event_string[128];
};

extern int register_thread_name(int unix_socket, char *thread_name);
extern int register_event(int unix_socket, int event_id);
extern int publish_event(int unix_socket, int event_id, int event_state);
extern int publish_event_string(int unix_socket, int event_id, char *event_string);
extern int subscribe_event(int unix_socket, int event_id);
extern int connect_event_mngr();
extern int parse_broadcast_event(char *buf, struct br_event_t *br_event);
extern int parse_broadcast_event_string(char *buf, struct br_event_string_t *br_event);
extern int parse_event_packet(struct event_packet_s *p_event_packet, int len);
extern int event_packet_dump(struct event_packet_s * p_event_packet);

#endif
