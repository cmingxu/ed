#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ed.h"

static int pack_config(config_t *c, char *);

int connect() {
  return CONNECT_SUCCESS;
}

int send_config() {
  return SEND_CONFIG_SUCCESS;
}

int write_config() {
  return WRITE_CONFIG_SUCCESS;
}

int read_config() {
  return READ_CONFIG_SUCCESS;
}

unsigned int package_count() {
  return 0;
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

static int pack_config(config_t *c, char *packed) {
  char code[8] = {0x44, 0xE1, 0xF4, 0x61, 0x43, 0xEF, 0xED, 0x65};
  char *pos = packed;
  memcpy(pos, code, 8);
  pos += 8;
  return 0;
}
