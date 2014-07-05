#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

enum protol_profile{
	PROFILE_light_ctrl = 0,/* 开关，亮度调节*/
	PROFILE_power_socket = 1, /* 开关，电能读取 */
	PROFILE_sensor = 2, /* 温度，湿度等 */
	PROFILE_air_conditioning = 3, /* 空调控制，开关，温度控制等*/
	PROFILE_end
};

/* Command define  */

/* 对于所有get 命令的返回消息 */
typedef struct {
	uint8_t cmd;
	uint16_t val;
}request_ack_t;


/* PROFILE_light_ctrl */
#define CMD_LIGHT_OPEN 0
#define CMD_LIGHT_CLOSE 1
#define CMD_LIGHT_ADJUST 2

typedef struct{
	uint8_t cmd:2;
	uint8_t action;
}light_ctrl_t;

/* PROFILE_power_socket */
#define CMD_POWERSOCK_OPEN 0
#define CMD_POWERSOCK_CLOSE 1
#define CMD_POWERSOCK_GETPOWER 2
#define CMD_POWERSOCK_GETVOLTAGE 3
#define CMD_POWERSOCK_GETCURRENT 4
typedef struct{
	uint8_t cmd:6;
	uint8_t  action:2;
}powersock_ctrl_t;

/* PROFILE_sensor */
#define CMD_SENSOR_GETTEMP 0


#endif /* __PROTOCOL_H__ */

