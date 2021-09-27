#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include "ed.h"
#include "utils.h"

// default buf size
static const unsigned int BUFSIZE = 2 << 10;
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
static size_t _write(addr_t *, char *buf, size_t);
static size_t _read(addr_t *, char *buf, size_t);
static void _settimeout(addr_t *, unsigned int);
static void _setbuf(addr_t *);
static unsigned int _sample_bytes_count(config_t *);

// 核心功能函数
int connect_device(config_t *c, addr_t *addr) {
  ED_LOG("connect device: %s", c->device_ip);
  assert(c);
  assert(addr);
  // clear servaddr
  bzero(&(addr->deviceaddr), sizeof(addr->deviceaddr));
  addr->deviceaddr.sin_family = AF_INET;
  addr->deviceaddr.sin_addr.s_addr = inet_addr(c->device_ip);
  addr->deviceaddr.sin_port = htons(c->device_port);


  bzero(&(addr->localaddr), sizeof(addr->localaddr));
  addr->localaddr.sin_family = AF_INET;
  addr->localaddr.sin_addr.s_addr = inet_addr(c->local_ip);
  addr->localaddr.sin_port = htons(c->local_port);

  // create datagram socket
  addr->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (bind(addr->socket, (struct sockaddr *) &(addr->localaddr), sizeof(addr->localaddr)) < 0) {
    ED_LOG("bind failed: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  // connect to device UDP endpoint
  if(connect(addr->socket, (struct sockaddr *)&addr->deviceaddr, sizeof(addr->deviceaddr)) < 0) {
    ED_LOG("connect failed: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  _setbuf(addr);

  // sending connect request
  char message[32];
  memset(message, '\0', 32);
  _pack_char_arr(message, CODE_CONNECT_REQURST, 8);
  if(_write(addr, message, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  _settimeout(addr, 100);
  // recv connect response
  char connect_resp[BUFSIZE];
  int nread;
  if((nread = _read(addr, connect_resp, BUFSIZE)) != 32) {
    ED_LOG("read faild: %d %s", nread, strerror(errno));
    return CONNECT_FAIL;
  }

  char expected[8];
  _pack_char_arr(expected, CODE_CONNECT_RESPONSE, 8);
  if(memcmp(connect_resp, expected, 8) != 0 ) {
    return CONNECT_VERIFY_ERR;
  }
  return CONNECT_SUCCESS;
}

int disconnect_device(config_t *c, addr_t *addr) {
  if(addr->socket != 0){
    close(addr->socket);
  }

  addr->socket = 0;
  return 1;
}

int send_config_to_device(config_t *c, addr_t *addr) {
  ED_LOG("send_config_to_device: %s", c->device_ip);

  char buf[32];
  memset(buf, '\0', 32);
  _pack_config(c, buf);
  _debug_hex(buf, 32);

  if(_write(addr, buf, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  _settimeout(addr, 100);
  // recv connect response
  char send_config_resp[32];
  if(_read(addr, send_config_resp, 32) != 32) {
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
int start_collect(config_t *c, addr_t *addr){
  ED_LOG("start_collect: %s", c->device_ip);

  char buf[32];
  memset(buf, '\0', 32);
  _pack_char_arr(buf, CODE_START_COLLECT, 8);
  _debug_hex(buf, 32);

  _settimeout(addr, 100);
  if(_write(addr, buf, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return START_COLLECT_FAIL;
  }

  return START_COLLECT_SUCCESS;
}

size_t start_recv(config_t *c, addr_t *addr, void *buf, size_t size){
  ED_LOG("start_recv: %s", c->device_ip);

  unsigned int received_byte_count = 0;
  unsigned int expected_byte_count = _sample_bytes_count(c) * c->repeat_count;
  unsigned int sample_index = 0;

  if(size < expected_byte_count) {
     ED_LOG("buf size not big enough, expected %d", expected_byte_count);
     goto end;
  }
  memset(buf, '\0', size);

  char tmp[MTU];
  _settimeout(addr, 200);

  // for each sample
  for (; sample_index < c->repeat_count; sample_index++) {
    int packet_count = ceil(_sample_bytes_count(c) / (float)MTU);
    unsigned int packet_index = 0;
    while(packet_index < packet_count) {
      memset(tmp, 0, MTU);
      int nread = _read(addr, tmp, MTU);

      if(nread == -1) {
        ED_LOG("start_recv failed: %s", strerror(errno));
        goto end;
      }

      // if packet size 32 and not last packet, should be stop_collect response
      if(nread == 32 && packet_index != ( packet_count -1)) {
        char expected[8];
        _pack_char_arr(expected, CODE_STOP_COLLECT_RESPONSE, 8);
        _debug_hex(tmp, 32);
        if(memcmp(tmp, expected, 8) != 0 ) {
          ED_LOG("stop_collect response received while receiving normal data %s", "");
          goto end;
        }
      }

      memcpy(buf + received_byte_count, tmp, nread);
      received_byte_count += nread;
      packet_index += 1;
    }
  }

  ED_LOG("total received bytes count %d\n", received_byte_count);
end:
  return received_byte_count;
}

void start_recv_with_file(config_t *c, addr_t *addr, FILE *file){
  ED_LOG("start_recv: %s", c->device_ip);

  unsigned int received_byte_count = 0;
  unsigned int expected_byte_count = _sample_bytes_count(c) * c->repeat_count;
  unsigned int sample_index = 0;

  char *cache = (char *)malloc(expected_byte_count);
  char buf[MTU];

  _settimeout(addr, 500);
  for (; sample_index < c->repeat_count; sample_index++) {
    int packet_count = ceil(_sample_bytes_count(c) / (float)MTU);
    unsigned int packet_index = 0;
    while(packet_index < packet_count) {
      memset(buf, 0, MTU);
      int nread = _read(addr, buf, MTU);

      if(nread == -1) {
        ED_LOG("start_recv failed: %s", strerror(errno));
        goto cleanup;
      }

      // if packet size 32 and not last packet, should be stop_collect response
      if(nread == 32 && packet_index != ( packet_count -1)) {
        char expected[8];
        _pack_char_arr(expected, CODE_STOP_COLLECT_RESPONSE, 8);
        _debug_hex(buf, 32);
        if(memcmp(buf, expected, 8) != 0 ) {
          ED_LOG("stop_collect response received while receiving normal data %s", "");
          goto cleanup;
        }
      }

      received_byte_count += nread;
      packet_index += 1;
    }
  }

  ED_LOG("total received bytes count %d\n", received_byte_count);
  if(c->storage_enabled) {
    fwrite(buf, 4096, received_byte_count, file);
  }
  fflush(file);

cleanup:
  free(cache);
  return;
}

// 停止采集
// NOTES: stop_collect response take more then 1s to return
int stop_collect(config_t *c, addr_t *addr){
  char buf[32];
  memset(buf, '\0', 32);
  _pack_char_arr(buf, CODE_STOP_COLLECT_REQUEST, 8);
  _debug_hex(buf, 32);

  _settimeout(addr, 100);
  _setbuf(addr);
  if(_write(addr, buf, 32) != 32) {
    ED_LOG("write faild: %s", strerror(errno));
    return STOP_COLLECT_FAIL;
  }

  _settimeout(addr, 0);
  // recv connect response
  char stop_collect_resp[32];
  if(_read(addr, stop_collect_resp, 32) != 32) {
    ED_LOG("read faild: %s", strerror(errno));
    return CONNECT_FAIL;
  }

  char expected[8];
  _pack_char_arr(expected, CODE_STOP_COLLECT_RESPONSE, 8);
  _debug_hex(stop_collect_resp, 32);
  if(memcmp(stop_collect_resp, expected, 8) != 0 ) {
    return SEND_CONFIG_VERIFY_ERR;
  }

  return STOP_COLLECT_SUCCESS;
}

unsigned int package_count(config_t *c) {
  ED_LOG("package_count: %s", c->device_ip);
  assert(c);

  unsigned single_repeat_bytes_count = c->sample_count * 2 * c->ad_channel;
  if (single_repeat_bytes_count % MTU == 0) {
    return single_repeat_bytes_count / MTU * c->repeat_count;
  }else{
    return ceil(single_repeat_bytes_count / (float)MTU)  * c->repeat_count;
  }
}

unsigned int bytes_count(config_t *c) {
  ED_LOG("bytes_count: %s", c->device_ip);
  assert(c);

  return c->sample_count * 2 * c->ad_channel * c->repeat_count;
}

int enable_cache(config_t *c) {
  assert(c);

  c->storage_enabled = true;
  return 1;
}

int disable_cache(config_t *c) {
  assert(c);

  c->storage_enabled = false;
  return 1;
}

//////////////////////////// STATIC FUNCTIONS ///////////////////
static unsigned int
_sample_bytes_count(config_t *c) {
  return  c->sample_count * 2 * c->ad_channel;
}


static int _pack_config(config_t *c, char *packed) {
  char *pos = packed;
  _pack_char_arr(pos, CODE_SEND_CONFIG_REQUEST, 8);
  pos+=8;

  _pack_uint32(pos, c->sample_count);
  pos+=4;

  _pack_uint32(pos, c->delay_count);
  pos+=4;

  _pack_uint16(pos, c->repeat_count);
  pos+=2;

  _pack_uint16(pos, c->down_sample_count);
  pos+=2;

  _pack_uint32(pos, ip_to_int(c->device_ip));
  pos+=4;

  _pack_short(pos, c->trigger);
  pos+=1;

  _pack_short(pos, c->outer_trigger);
  pos+=1;

#ifdef NEW_LOCAL_IP
  _pack_uint32(pos, ip_to_int(c->new_local_ip));
  pos+=4;
#endif

  return 0;
}

static size_t _write(addr_t *addr, char *buf, size_t size){
  ED_LOG("write %lu bytes to %d", size, addr->socket);
  return sendto(addr->socket, buf, size, 0, (struct sockaddr*)NULL, sizeof(addr->deviceaddr));
}

static size_t _read(addr_t *addr, char *buf, size_t size){
  /*ED_LOG("recvfrom %d", addr->socket);*/
  return recvfrom(addr->socket, buf, size, 0, (struct sockaddr*)NULL, NULL);
}

static void _settimeout(addr_t *addr, unsigned int milliseconds) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1000 * milliseconds;
  if (setsockopt(addr->socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    ED_LOG("setsockopt timeout error: %s", strerror(errno));
  }
}

static void _setbuf(addr_t *addr) {
  int n = 4 * 1024 * 1024; // 20M
  if (setsockopt(addr->socket, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
    ED_LOG("setsockopt buffer error: %s", strerror(errno));
  }
}
