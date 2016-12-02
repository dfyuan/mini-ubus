#ifndef _SAM_CONF_FILE_H
#define _SAM_CONF_FILE_H

#define PARAMS_MAGIC            0xdeadbeaf
#define TYPE_PARAM_CONF_FILE    0x11
#define TYPE_ATTR_CONF_FILE     0x12
#define MAX_FILE_SIZE           4096

#define PATH_DEFAULT_CONF_HEAD     "/root/sam_conf/"
#define PATH_USR_CONF_HEAD         "/root/data/sam_conf/"
#define PATH_PARAM_TAIL            "/param_config/"
#define PATH_ATTR_TAIL             "/attr_config/"
#define SAM_ATTR_CONF_FILE         "sam_attr_conf"

#pragma pack(2)
struct curr_params_conf {
	char curr_sam_conf_name[128];
	char path_usr_param_conf[128];
	char path_default_param_conf[128];
	char path_usr_attr_conf[128];
	char path_default_attr_conf[128];
	char path_sam_attr_conf[128];
};

struct params_sam5xxx_header_s {
	unsigned char crc;
	int magic;
	int type;
	int id;
	char conf_name[64];
};

struct sam_attr_body_s {
	int curr_conf_fileId;
};

struct sam_attr_conf_s {
	struct params_sam5xxx_header_s params_sam5xxx_header;
	struct sam_attr_body_s sam_attr_body;
};
#pragma pack(0)

int check_sam_conf_file_valid(char *file_name);
int get_conf_file_header(char *file_name, struct params_sam5xxx_header_s *params_sam_header);
int set_conf_file_header(char *file_name, struct params_sam5xxx_header_s *params_sam_header);
int get_attr_conf_body(struct sam_attr_body_s *sam_attr_body, char *sam_attr_conf_path);
int set_attr_conf_body(struct sam_attr_body_s *sam_attr_body, char *sam_attr_conf_path);
int get_curr_conf_fileId(char *sam_attr_conf_path);
int update_curr_conf_fileId(int fileId, char *sam_attr_conf_path);
int get_sam_conf_filename_byId(char *file_name, int file_id, char *usr_param_conf_path);
int get_curr_sam_conf_filename(char *file_name, char *sam_attr_conf_path, char *usr_param_conf_path);
unsigned char calc_crc(unsigned char *p, unsigned long counter);

#endif
