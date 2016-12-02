#ifndef GLOBAL_STATE_H_
#define GLOBAL_STATE_H_

#define BROADCAST_WIFI_EVENT_ID                 0x01
#define BROADCAST_INPUT_MODE_EVENT_ID           0x02
#define BROADCAST_SOUND_VOLUME_EVENT_ID         0x03
#define BROADCAST_UPGRADE_EVENT_ID              0x04
#define BROADCAST_SONA_APP_CMD_EVENT_ID         0x05
#define BROADCAST_PROMPT_SOUND_EVENT_ID         0x06
#define BROADCAST_UPGRADE_PROMPT_SOUND_EVENT_ID 0x07
#define BROADCAST_MIC_EQ_EVENT_ID               0x08
#define BROADCAST_MIC_STATE_EVENT_ID            0x09
#define BROADCAST_HEADSET_STATE_EVENT_ID        0x10
#define BROADCAST_PLAY_CTRL_EVENT_ID            0x11
#define BROADCAST_WORK_MODE_EVENT_ID            0x12
#define BROADCAST_WIFI_PLAYLIST_EVENT_ID        0x13
#define BROADCAST_MUSIC_PLAY_STATE_EVENT_ID     0x14
#define BROADCAST_ETH_EVENT_ID                  0x15
#define BROADCAST_KEY_EVENT_ID                  0x16
#define BROADCAST_LCD_EVENT_ID                  0x17

#define LCD_LOCK_DEVICE       0
#define LCD_UNLOCK_DEVICE     1

#define PLAY_CTRL_PLAYPAUSE                     1
#define PLAY_CTRL_PRESONG                       2
#define PLAY_CTRL_NEXTSONG                      3
#define PLAY_CTRL_VOLUMEUP                      4
#define PLAY_CTRL_VOLUMEDOWN                    5
#define PLAY_CTRL_PLAYLISTSW                    6
#define PLAY_CTRL_MICEQSW                       7
#define PLAY_CTRL_INPUTMODESW                   8
#define PLAY_CTRL_WORKMODESW                    9
#define PLAY_CTRL_WIFIMODESW                    10
#define PLAY_CTRL_WORKMODEFAC                   11
#define PLAY_CTRL_PLAY                          12
#define PLAY_CTRL_PAUSE                         13

#define WORK_MODE_NORMAL                        1
#define WORK_MODE_STANDBY                       2
#define WORK_MODE_FACTORY                       3

#define BROADCAST_WIFI_CFG_EVENT_ID             0x50
#define BROADCAST_SDC_INFO_EVENT_ID             0x51
#define BROADCAST_WIFI_CFG_TRIGGER_EVENT_ID     0x52
#define BROADCAST_UPGRADE_PROCESS_EVENT_ID      0x53		/* 0 ~ 100 */

/* wlan state */
#define WLAN_STATE_DISCONNECTED        0x01
#define WLAN_STATE_CONNECTING_AP       0x02
#define WLAN_STATE_GET_DNS             0x03
#define WLAN_STATE_CONNECTED           0x04
#define WLAN_STATE_SNIFFER             0x05
#define WLAN_STATE_CONNECTING_START    0x10
#define WLAN_STATE_CONNECT_FAILED      0x11

/* eth state */
#define ETH_STATE_DISCONNECTED         0x01
#define ETH_STATE_CONNECTING           0x02
#define ETH_STATE_CONNECTED            0x03
#define ETH_STATE_EXCEPTION            0x04

/* input mode state */
#define INPUT_SRC_MODE_WIFI      0x21
#define INPUT_SRC_MODE_BT        0x22
#define INPUT_SRC_MODE_AUX       0x23
#define INPUT_SRC_MODE_TF        0x24
#define INPUT_SRC_MODE_RCA       0x25
#define INPUT_SRC_MODE_OPTI      0x26
#define INPUT_SRC_MODE_COXA      0x27
#define INPUT_SRC_MODE_HDMI1     0x28
#define INPUT_SRC_MODE_HDMI2     0x29
#define INPUT_SRC_MODE_ARC       0x2a
#define INPUT_SRC_MODE_SPDIF     0x2b
#define INPUT_SRC_MODE_AP        0x2c

/* upgrade state */
#define UPGRADE_STATE_START     0x51
#define UPGRADE_STATE_SUCCESS	0x52
#define UPGRADE_STATE_FIALED	0x53

/* ap mode state changed */
#define AP_MODE_CHANGED         0x70

/* soundbar mic state */
#define MIC_STATE_IN					0x101
#define MIC_STATE_OUT					0x102

#define MUSIC_STATE_PLAY       0x01
#define MUSIC_STATE_PAUSE      0x02

/* wifi cfg state */
#define WIFI_CFG_START          0x501
#define WIFI_CFG_OK             0x502
#define WIFI_CFG_FAILED         0x503

/* sd card state */
#define SD_CARD_PLUGIN          0x511
#define SD_CARD_PLUGOUT         0x512

/* sona发送网络配置状态消息类型 */
#define PLS_START_WIFI_WPS_CFG            0x521
#define PLS_START_WIFI_OMG_CFG            0x522
#define PLS_START_WIFI_RECONNECT          0x523

struct global_device_state_t {
	int wlan_state;
	int eth0_state;
	int input_src_mode;
	int music_volume;
	int prompt_volume;
	int ap_state;
	int sdc_state;
	int upgrade_state;
};

extern struct global_device_state_t global_device_state;

#endif
