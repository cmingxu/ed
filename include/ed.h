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

struct repeat_response {
  // 设备采样数据
  void *data;
  // 接受data_size字节数， 此sample的其余丢弃
  size_t data_size;
  // 从设备收集到的数据大小
  size_t recv_data_size;
  
  // 应该收到的包总量
  size_t packet_count;
  // 实际收到包总量
  size_t recv_packet_count;
};
typedef struct repeat_response repeat_response_t;

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
  uint16_t down_sample_count;
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

  // 新IP
  char new_local_ip[32];
  // 新port
  unsigned int new_local_port;
};
typedef struct config config_t;

// create default global config
int load_default_config(config_t *);
int set_local_addr(config_t *, const char*, unsigned int);
int set_device_addr(config_t *, const char*, unsigned int);

int set_new_local_addr(config_t *, const char*, unsigned int);


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
unsigned int packet_count_of_each_repeat(config_t *c);

// 计算数据bytecount
unsigned int bytes_count(config_t *);

// 计算数据每个repeat的数据大小
unsigned int repeat_bytes_count(config_t *);

// 计算数据每个repeat的数据大小heading includes
unsigned int repeat_bytes_count_with_heading(config_t *);

// 初始化socket链接
#define ESTABLISH_CONNECTION_SUCCESS 0
#define ESTABLISH_CONNECTION_BIND_FAIL 10001
#define ESTABLISH_CONNECTION_CONNECT_FAIL 10002
int establish_connection(config_t *t, addr_t *);
// 清理socket链接
int teardown_connection(config_t *, addr_t *);

// 内部触发
#define START_COLLECT_SUCCESS 0
#define START_COLLECT_FAIL 10001
#define START_COLLECT_TIMEOUT 10002
#define START_COLLECT_VERIFY_ERR 10003
int start_collect(config_t *, addr_t *);

// 停止采集
#define STOP_COLLECT_SUCCESS 0
#define STOP_COLLECT_FAIL 10001
#define STOP_COLLECT_VERIFY_FAIL 10002
int stop_collect(config_t *, addr_t *);

// 开始数据接收 - 每次读取一个
// @param - config_t 配置
// @param - addr_t 地址
// @param - repeat_response_t 回包结构，data字段需要提前申请内存
// @param - repeat 从0 - c->repeat_count顺序增加，如果从设备接受的packet不属于此范围则丢弃
// @param - timeout read timeout
#define START_RECV_SUCCESS 10000
#define START_RECV_PACKET_LOSS 10001
#define START_RECV_INVALID_PACKET_LEN 10002
int start_recv_by_repeat(config_t *, addr_t *, repeat_response_t *, unsigned int repeat, unsigned int timeout);


#ifdef ED_DEBUG
#define MYLOG(fmt, ...) do { \
		fprintf(stdout, "[LOG] %s:%d: " fmt "\n", __FUNCTION__,__LINE__,__VA_ARGS__); \
		fflush(stdout); \
} while(0);

#define ED_LOG( fmt, ... ) MYLOG(fmt,__VA_ARGS__)
#else
#define ED_LOG( fmt, ... ) {}
#endif

#ifdef __cplusplus
}
#endif
#endif
