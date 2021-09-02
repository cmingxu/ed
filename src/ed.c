#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ed.h"
#include "utils.h"

static const unsigned int MTU = 1472;

// 指令码
static const char CODE_CONNECT_REQURST[8] = {0x31, 0xB3, 0x59, 0xC6, 0x47, 0xD8, 0x5A, 0xC3};
static const char CODE_CONNECT_RESPONSE[8] = {0x21, 0x74, 0x63, 0x65, 0x6E, 0x4E, 0x6F, 0x43};
static const char CODE_SEND_CONFIG_REQUEST[8] = {0x44, 0xE1, 0xF4, 0x61, 0x43, 0xEF, 0xED, 0x65};
static const char CODE_SEND_CONFIG_RESPONSE[8] = {0x23, 0x76, 0x63, 0x52, 0x20, 0x67, 0x66, 0x43};
static const char CODE_TRIGGER[8] = {0x67, 0x69, 0x72, 0x54, 0x2D, 0x74, 0x6E, 0x49};
static const char CODE_STOP_COLLECT_REQUEST[8] = {0x71, 0x63, 0x41, 0x2B, 0x70, 0x6F, 0x74, 0x53};
static const char CODE_STOP_COLLECT_RESPONSE[8] = {0x50, 0x6F, 0x74, 0x73, 0x3E, 0x6E, 0x69, 0x46};

static int _pack_config(config_t *, char *);

// 核心功能函数
int connect() {
  return CONNECT_SUCCESS;
}

int send_config() {
  char buf[32];
  memset(buf, '\0', 32);
  _pack_config(g_config, buf);

  return SEND_CONFIG_SUCCESS;
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

int enable_cache() {
  assert(g_config);

  g_config->cache_enabled = true;
  return 1;
}

int disable_cache() {
  assert(g_config);
  
  g_config->cache_enabled = false;
  return 1;
}

int _pack_config(config_t *c, char *packed) {
  char *pos = packed;
  for (int i = 0; i < 8; i++) {
    *(pos++) = CODE_CONNECT_REQURST[i];
  }

  _pack_uint32(pos, c->sampleCount, 4);
  pos+=4;

  _pack_uint32(pos, c->delayCount, 4);
  pos+=4;

  _pack_uint16(pos, c->repeatCount, 2);
  pos+=2;

  _pack_uint16(pos, c->sampleCount2, 2);
  pos+=2;

  _pack_uint32(pos, c->localIp, 4);
  pos+=4;

  _pack_short(pos, c->trigger, 1);
  pos+=1;

  _pack_short(pos, c->outer_trigger, 1);
  pos+=1;

  for (int i = 0; i < (pos - packed); i++) {
    ED_LOG("%02x", packed[i] & 0xff);
  }
  return 0;
}

