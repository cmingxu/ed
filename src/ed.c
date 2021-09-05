#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <errno.h>

#include "ed.h"
#include "utils.h"

static const unsigned int MTU = 1472;

// 指令码
static const char CODE_CONNECT_REQURST[8] = {0x31, 0xB3, 0x59, 0xC6, 0x47, 0xD8, 0x5A, 0xC3};
static const char CODE_CONNECT_RESPONSE[8] = {0x21, 0x74, 0x63, 0x65, 0x6E, 0x4E, 0x6F, 0x43};
static const char CODE_SEND_CONFIG_REQUEST[8] = {0x44, 0xE1, 0xF4, 0x61, 0x43, 0xEF, 0xED, 0x65};
static const char CODE_SEND_CONFIG_RESPONSE[8] = {0x23, 0x76, 0x63, 0x52, 0x20, 0x67, 0x66, 0x43};
static const char CODE_START_COLLECT[8] = {0x67, 0x69, 0x72, 0x54, 0x2D, 0x74, 0x6E, 0x49};
static const char CODE_STOP_COLLECT_REQUEST[8] = {0x71, 0x63, 0x41, 0x2B, 0x70, 0x6F, 0x74, 0x53};
static const char CODE_STOP_COLLECT_RESPONSE[8] = {0x50, 0x6F, 0x74, 0x73, 0x3E, 0x6E, 0x69, 0x46};

static int _pack_config(config_t *, char *);
static size_t _write(char *buf, size_t);
static size_t _read(char *buf, size_t);
static void _settimeout(unsigned int);
static void _setbuf();
static unsigned int bytes_count();

// 核心功能函数
int connect_device() {
  // clear servaddr
  bzero(&g_config->addr->deviceaddr, sizeof(g_config->addr->deviceaddr));
  g_config->addr->deviceaddr.sin_addr.s_addr = inet_addr(g_config->deviceIp);
  g_config->addr->deviceaddr.sin_port = htons(g_config->devicePort);
  g_config->addr->deviceaddr.sin_family = AF_INET;

  bzero(&g_config->addr->localaddr, sizeof(g_config->addr->localaddr));
  g_config->addr->localaddr.sin_family = AF_INET;
  g_config->addr->localaddr.sin_addr.s_addr = inet_addr(g_config->localIp);
  g_config->addr->localaddr.sin_port = htons(g_config->localPort);

  // create datagram socket
  g_config->addr->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (bind(g_config->addr->socket, (struct sockaddr *) &g_config->addr->localaddr, sizeof(g_config->addr->localaddr)) < 0) {
    ED_LOG("bind failed: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  // connect to device UDP endpoint
  if(connect(g_config->addr->socket, (struct sockaddr *)&g_config->addr->deviceaddr, sizeof(g_config->addr->deviceaddr)) < 0)
  {
    ED_LOG("connect failed: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  // sending connect request
  char message[32];
  memset(message, '\0', 32);
  _pack_char_arr(message, CODE_CONNECT_REQURST, 8);
  if(_write(message, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  // recv connect response
  char connect_resp[32];
  if(_read(connect_resp, 32) != 32) {
    ED_LOG("read faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  char expected[8];
  _pack_char_arr(expected, CODE_CONNECT_RESPONSE, 8);
  if(memcmp(connect_resp, expected, 8) != 0 ) {
    return CONNECT_VERIFY_ERR;
  }
  return CONNECT_SUCCESS;
}

int send_config() {
  char buf[32];
  memset(buf, '\0', 32);
  _pack_config(g_config, buf);
  _debug_hex(buf, 32);

  if(_write(buf, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  // recv connect response
  char send_config_resp[32];
  if(_read(send_config_resp, 32) != 32) {
    ED_LOG("read faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  char expected[8];
  _pack_char_arr(expected, CODE_SEND_CONFIG_RESPONSE, 8);
  _debug_hex(send_config_resp, 32);
  if(memcmp(send_config_resp, expected, 8) != 0 ) {
    return SEND_CONFIG_VERIFY_ERR;
  }

  return SEND_CONFIG_SUCCESS;
}


// 内部触发命令
int start_collect(){
  char buf[32];
  memset(buf, '\0', 32);
  _pack_char_arr(buf, CODE_START_COLLECT, 8);
  _debug_hex(buf, 32);

  /*_settimeout(100);*/
  _setbuf();
  if(_write(buf, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return START_COLLECT_FAIL;
  }

  return START_COLLECT_SUCCESS;
}

void start_recv(){
  unsigned int total_bytes = bytes_count(), 
               received = 0;
  char buf[MTU];
  int iteration = ceil(total_bytes / (float)MTU), 
      current_it = 0;

  /*_settimeout(500);*/
  printf("\ntotal expected %d\n", total_bytes);
  printf("\niteration expected%d\n", iteration);
  while(current_it < iteration) {
    memset(buf, '\0', MTU);
    int nread = _read(buf, MTU);
    received += nread;
  /*printf("nread %d\n", nread);*/
  /*printf("received %d\n", received);*/
  /*printf("total_iteration %d\n", current_it);*/
    /*_debug_hex(buf, MTU);*/
    current_it += 1;
    if(nread < MTU) {
      /*break;*/
      printf("11111111111\n");
  printf("nread %d\n", nread);
  printf("received %d\n", received);
  printf("total_iteration %d\n", current_it);
    }
  }

  printf("received %d\n", received);
  printf("total_iteration %d\n", current_it);
}

unsigned int package_count() {
  assert(g_config);

  unsigned single_repeat_bytes_count = g_config->sampleCount * 2 * g_config->ad_channel;
  if (single_repeat_bytes_count % MTU == 0) {
    return single_repeat_bytes_count / MTU * g_config->repeatCount;
  }else{
    return ceil(single_repeat_bytes_count / (float)MTU)  * g_config->repeatCount;
  }
}

static unsigned int
bytes_count() {
  return  g_config->sampleCount * 2 * g_config->ad_channel * g_config->repeatCount;
}

int enable_cache() {
  assert(g_config);

  g_config->storage_enabled = true;
  return 1;
}

int disable_cache() {
  assert(g_config);

  g_config->storage_enabled = false;
  return 1;
}

static int _pack_config(config_t *c, char *packed) {
  char *pos = packed;
  _pack_char_arr(pos, CODE_SEND_CONFIG_REQUEST, 8);
  pos+=8;

  _pack_uint32(pos, c->sampleCount);
  pos+=4;

  _pack_uint32(pos, c->delayCount);
  pos+=4;

  _pack_uint16(pos, c->repeatCount);
  pos+=2;

  _pack_uint16(pos, c->sampleCount2);
  pos+=2;

  _pack_uint32(pos, config_device_ip_int32());
  pos+=4;

  _pack_short(pos, c->trigger);
  pos+=1;

  _pack_short(pos, c->outer_trigger);
  pos+=1;

  return 0;
}

static size_t _write(char *buf, size_t size){
  return sendto(g_config->addr->socket, buf, size, 0, (struct sockaddr*)NULL, sizeof(g_config->addr->deviceaddr));
}

static size_t _read(char *buf, size_t size){
  return recvfrom(g_config->addr->socket, buf, size, 0, (struct sockaddr*)NULL, NULL);
}

static void _settimeout(unsigned int milliseconds) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1000 * milliseconds;
  if (setsockopt(g_config->addr->socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    ED_LOG("setsockopt timeout error: %s", strerror(errno));
  }
}

static void _setbuf() {
  int n = 10 * 1024 * 1024; // 20M
  if (setsockopt(g_config->addr->socket, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
    ED_LOG("setsockopt buffer error: %s", strerror(errno));
  }
}
