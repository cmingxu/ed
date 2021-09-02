#ifndef ED_H
#define ED_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct config{
  // 采样点数
  uint32_t sampleCount;
  // 延时点数
  uint32_t delayCount;
  // 重复次数
  uint16_t repeatCount;
  // 降采样点数
  uint16_t sampleCount2;
  // 本机IP地址
  int32_t localIp;
  // 本机UDP端口地址
  unsigned int localPort;
  // 设备IP地址
  int32_t deviceIp;
  // 设备端口地址
  unsigned int devicePort;

  short ad_channel;
  short ad_bit;
  short trigger;
  short outer_trigger;

  // log file fd, default stdout
  FILE* log_file;
  FILE* config_file;
  int socket;
  bool cache_enabled;

  // marks config written to device or not
  bool dity;
};
typedef struct config config_t;

// create default global config
config_t* create_default_config();
config_t* update_log_file(FILE *);
config_t* update_config_file(FILE *);
config_t* update_local_addr(const char*, unsigned int);
config_t* update_device_addr(const char*, unsigned int);

// 全局配置
config_t* g_config;

// AD 通道数 默认 1(1:单通道;2:双通道);
#define ADCHANNEL_SINGLE 1
#define ADCHANNEL_BIDIRECTIONAL 2

// AD 位数，无符号，1Byte，默认 12(12:12bit;14:14bit); 
#define ADBIT_12 12
#define ADBIT_14 14

// 触发方式，无符号，1Byte，默认 0(0x0:外触发，0x1:内触发);
#define TRIGGER_UP 0
#define TRIGGER_DOWN 1

// 外触发边沿，无符号，1Byte，默认 0(0x0:下降沿，0x1:上升沿);
#define OUTER_TRIGGER_DOWN 0
#define OUTER_TRIGGER_UP 1


#define CONNECT_SUCCESS 0
#define CONNECT_FAIL 10001
#define CONNECT_TIMEOUT 10002
#define CONNECT_VERIFY_ERR 10003
int connect();

// 发送配置信息
#define SEND_CONFIG_SUCCESS 0
#define SEND_CONFIG_FAIL 10001
#define SEND_CONFIG_TIMEOUT 10002
#define SEND_CONFIG_VERIFY_ERR 10003
int send_config();

// 存储配置信息
#define WRITE_CONFIG_FAIL -1
#define WRITE_CONFIG_SUCCESS 0
int write_config();

// 读取配置信息
#define READ_CONFIG_SUCCESS 0
#define READ_CONFIG_FAIL -1
int read_config();

// 计算数据包数量
unsigned int package_count();

// 使能数据存储
int enable_cache();

// 禁用数据存储
int disable_cache();

// 内部触发
// 停止采集
int stop_collect();

// 开始数据接收
void start_recv();

// 停止数据接收
void stop_recv();

// 重新开始数据接收
void restart_recv();


#define LOG(fmt, ...) do {	\
		fprintf(g_config->log_file, "[LOG] %s:%d: " fmt "\n", __FUNCTION__,__LINE__, __VA_ARGS__); \
		fflush(g_config->log_file); \
} while(0)
 
#define ED_LOG( fmt, ... ) LOG(fmt,__VA_ARGS__ )
#endif
