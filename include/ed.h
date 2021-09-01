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
  uint32_t repeatCount;
  // 降采样点数
  uint32_t sampleCount2;
  // 本机IP地址
  int32_t localIp;
  // 本机UDP端口地址
  unsigned int localPort;
  // 设备IP地址
  int32_t deviceIp;
  // 设备端口地址
  unsigned int devicePort;

  short ADChannel;
  short ADBit;
  short trigger;

  // log file fd, default stdout
  int log_file_fd;
  int socket;
  bool cache_enabled;
};
typedef struct config config_t;

// create default global config
config_t* create_default_config();

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
#define OUTTER_TRIGGER_UP 0
#define OUTTER_TRIGGER_DOWN 1


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

#endif
