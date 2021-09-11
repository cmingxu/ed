#ifndef ED_H
#define ED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct addr{
  int socket;
  struct sockaddr_in deviceaddr;
  struct sockaddr_in localaddr;
};
typedef struct addr addr_t;

struct config{
  // 采样点数
  uint32_t sample_count;
  // 延时点数
  uint32_t delay_count;
  // 重复次数
  uint16_t repeat_count;
  // 降采样点数
  uint16_t sample_count2;
  // 本机IP地址
  char local_ip[32];
  // 本机UDP端口地址
  unsigned int local_port;
  // 设备IP地址
  char device_ip[32];
  // 设备端口地址
  unsigned int device_port;

  short ad_channel;
  short ad_bit;
  short trigger;
  short outer_trigger;

  bool storage_enabled;
};
typedef struct config config_t;

// create default global config
int load_default_config(config_t *);
int set_local_addr(config_t *, const char*, unsigned int);
int set_device_addr(config_t *, const char*, unsigned int);


// AD 通道数 默认 1(1:单通道;2:双通道);
#define ADCHANNEL_SINGLE 1
#define ADCHANNEL_BIDIRECTIONAL 2

// AD 位数，无符号，1Byte，默认 12(12:12bit;14:14bit); 
#define ADBIT_12 12
#define ADBIT_14 14

// 触发方式，无符号，1Byte，默认 0(0x0:外触发，0x1:内触发);
#define TRIGGER_OUTER 0
#define TRIGGER_INNER 1

// 外触发边沿，无符号，1Byte，默认 0(0x0:下降沿，0x1:上升沿);
#define OUTER_TRIGGER_DOWN 0
#define OUTER_TRIGGER_UP 1

// 建立连接
#define CONNECT_SUCCESS 0
#define CONNECT_FAIL 10001
#define CONNECT_TIMEOUT 10002
#define CONNECT_VERIFY_ERR 10003
int connect_device(config_t *, addr_t *);

// 断开连接
int disconnect_device(config_t *, addr_t *);

// 发送配置信息
#define SEND_CONFIG_SUCCESS 0
#define SEND_CONFIG_FAIL 10001
#define SEND_CONFIG_TIMEOUT 10002
#define SEND_CONFIG_VERIFY_ERR 10003
int send_config_to_device(config_t *, addr_t *);

// 存储配置信息
#define WRITE_CONFIG_FAIL -1
#define WRITE_CONFIG_SUCCESS 0
int write_config(config_t*, FILE *);

// 读取配置信息
#define READ_CONFIG_SUCCESS 0
#define READ_CONFIG_FAIL -1
int load_config(config_t*, FILE *);

// 计算数据包数量
unsigned int package_count(config_t *);

// 使能数据存储
int enable_cache(config_t *);

// 禁用数据存储
int disable_cache(config_t *);

// 内部触发
#define START_COLLECT_SUCCESS 0
#define START_COLLECT_FAIL 10001
#define START_COLLECT_TIMEOUT 10002
#define START_COLLECT_VERIFY_ERR 10003
int start_collect(config_t *, addr_t *);

// 停止采集
#define STOP_COLLECT_SUCCESS 0
#define STOP_COLLECT_FAIL 10001
int stop_collect(config_t *, addr_t *);

// 开始数据接收
void start_recv(config_t *, addr_t *, FILE *);

#define MYLOG(fmt, ...) do {	\
		fprintf(stdout, "[LOG] %s:%d: " fmt "\n", __FUNCTION__,__LINE__, __VA_ARGS__); \
		fflush(stdout); \
} while(0)

#ifdef ED_DEBUG
#define ED_LOG( fmt, ... ) MYLOG(fmt,__VA_ARGS__ )
#else
#define ED_LOG( fmt, ... ) {}
#endif

#ifdef __cplusplus
}
#endif
#endif
