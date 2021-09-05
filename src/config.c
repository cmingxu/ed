#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>

#include "ed.h"
#include "utils.h"

static const char *defaultLocalIP = "192.168.1.5";
static const unsigned int defaultLocalPort = 7;

static const char *defaultDeviceIP = "192.168.1.41";
static const unsigned int defaultDevicePort = 5000;

// config keys
static const char *KEY_SAMPLE_COUNT = "sample_count";
static const char *KEY_DELAY_COUNT = "delay_count";
static const char *KEY_REPEAT_COUNT = "repeat_count";
static const char *KEY_SAMPLE_COUNT2 = "sample_count2";
static const char *KEY_LOCAL_IP = "local_ip";
static const char *KEY_LOCAL_PORT = "local_port";
static const char *KEY_DEVICE_IP = "device_ip";
static const char *KEY_DEVICE_PORT = "device_port";
static const char *KEY_AD_CHANNEL = "ad_channel";
static const char *KEY_AD_BIT = "ad_bit";
static const char *KEY_TRIGGER = "trigger";
static const char *KEY_OUTER_TRIGGER = "outer_trigger";

static uint32_t ip_to_int(const char*);
static char* int_to_ip(char *, uint16_t);
static bool _write_config_prop_uint32(const char *, uint32_t);
static bool _write_config_prop_uint16(const char *, uint16_t);
static bool _write_config_prop_unsigned(const char *, unsigned int);
static bool _write_config_prop_short(const char *, short);
static bool _write_config_prop_str(const char *, const char*);

static bool _read_config_prop_uint32(const char *, uint32_t *);
static bool _read_config_prop_uint16(const char *, uint16_t *);
static bool _read_config_prop_unsigned(const char *, unsigned int *);
static bool _read_config_prop_short(const char *, short *);
static bool _read_config_prop_str(const char *, char *);

static bool has_prefix(const char*, const char *) ;


config_t *create_default_config() {
  if( g_config != NULL) {
    return g_config;
  }

  void *ptr = malloc(sizeof(config_t));
  if(ptr == NULL) {
    return g_config;
  }
  g_config = (config_t*)ptr;
  g_config->sampleCount = 1000000;
  g_config->delayCount = 320000;
  g_config->repeatCount = 3;
  g_config->sampleCount2 = 2;

  g_config->localIp = strdup(defaultLocalIP);
  g_config->localPort = defaultLocalPort;

  g_config->deviceIp = strdup(defaultDeviceIP);
  g_config->devicePort = defaultDevicePort;

  g_config->ad_channel = ADCHANNEL_SINGLE;
  g_config->ad_bit = ADBIT_12;
  g_config->trigger = TRIGGER_UP;
  g_config->outer_trigger = OUTER_TRIGGER_DOWN;

  // default set to stdout
  g_config->log_file = fdopen(STDOUT_FILENO, "a");
  g_config->config_file = NULL;

  g_config->addr = (ed_addr_t *)malloc(sizeof(ed_addr_t));

  return g_config;
}

// update log file handle
config_t *update_config_log_file(FILE *new_log_file) {
  g_config->log_file = new_log_file;
  return g_config;
}

// update config file
config_t *update_config_file(FILE *new_config_file) {
  g_config->config_file = new_config_file;
  return g_config;
}

// update local addr, local ip and local port update respectively
config_t *update_local_addr(const char*ip, unsigned int port) {
  g_config->localIp = strdup(ip);
  g_config->localPort = port;
  return g_config;
}

// update local addr, local ip and local port update respectively
config_t *update_device_addr(const char*ip, unsigned int port) {
  g_config->deviceIp = strdup(ip);
  g_config->devicePort = port;
  return g_config;
}

int write_config() {
  assert(g_config);
  assert(g_config->config_file);

  char local_ip_buf[128], device_ip_buf[128];
  memset(local_ip_buf, '\0', 128);
  memset(device_ip_buf, '\0', 128);

  ftruncate(fileno(g_config->config_file), 0);
  _write_config_prop_uint32(KEY_SAMPLE_COUNT, g_config->sampleCount);
  _write_config_prop_uint32(KEY_DELAY_COUNT, g_config->delayCount);
  _write_config_prop_uint16(KEY_REPEAT_COUNT, g_config->repeatCount);
  _write_config_prop_uint32(KEY_SAMPLE_COUNT2, g_config->sampleCount);
  _write_config_prop_str(KEY_LOCAL_IP, g_config->localIp);
  _write_config_prop_unsigned(KEY_LOCAL_PORT, g_config->localPort);
  _write_config_prop_str(KEY_DEVICE_IP, g_config->deviceIp);
  _write_config_prop_unsigned(KEY_DEVICE_PORT, g_config->devicePort);
  _write_config_prop_short(KEY_AD_CHANNEL, g_config->ad_channel);
  _write_config_prop_short(KEY_AD_BIT, g_config->ad_bit);
  _write_config_prop_short(KEY_TRIGGER, g_config->trigger);
  _write_config_prop_short(KEY_OUTER_TRIGGER, g_config->outer_trigger);
  fflush(g_config->config_file);
  return WRITE_CONFIG_SUCCESS;
}

int read_config() {
  assert(g_config);
  assert(g_config->config_file);

  char local_ip_buf[128], device_ip_buf[128];
  memset(local_ip_buf, '\0', 128);
  memset(device_ip_buf, '\0', 128);

  char buf[128];
  rewind(g_config->config_file);
  while(fgets(buf, 128, g_config->config_file) != NULL){
    if(has_prefix(buf, KEY_SAMPLE_COUNT)) {
      _read_config_prop_uint32(buf, &g_config->sampleCount);
    }

    if(has_prefix(buf, KEY_DELAY_COUNT)) {
      _read_config_prop_uint32(buf, &g_config->delayCount);
    }

    if(has_prefix(buf, KEY_REPEAT_COUNT)) {
      _read_config_prop_uint16(buf, &g_config->repeatCount);
    }

    if(has_prefix(buf, KEY_SAMPLE_COUNT2)) {
      _read_config_prop_uint16(buf, &g_config->sampleCount2);
    }

    if(has_prefix(buf, KEY_LOCAL_IP)) {
      _read_config_prop_str(buf, g_config->localIp);
    }

    if(has_prefix(buf, KEY_LOCAL_PORT)) {
      _read_config_prop_unsigned(buf, &g_config->localPort);
    }

    if(has_prefix(buf, KEY_DEVICE_IP)) {
      _read_config_prop_str(buf, g_config->deviceIp);
    }

    if(has_prefix(buf, KEY_DEVICE_PORT)) {
      _read_config_prop_unsigned(buf, &g_config->devicePort);
    }

    if(has_prefix(buf, KEY_AD_CHANNEL)) {
      _read_config_prop_short(buf, &g_config->ad_channel);
    }

    if(has_prefix(buf, KEY_AD_BIT)) {
      _read_config_prop_short(buf, &g_config->ad_bit);
    }

    if(has_prefix(buf, KEY_TRIGGER)) {
      _read_config_prop_short(buf, &g_config->trigger);
    }

    if(has_prefix(buf, KEY_OUTER_TRIGGER)) {
      _read_config_prop_short(buf, &g_config->outer_trigger);
    }
  }

  return READ_CONFIG_SUCCESS;
}


static uint32_t
ip_to_int(const char *ip){
  int32_t v = 0;
  int i;
  const char * start;

  start = ip;
  for (i = 0; i < 4; i++) {
    char c;
    int n = 0;
    while (1) {
      c = * start;
      start++;
      if (c >= '0' && c <= '9') {
        n *= 10;
        n += c - '0';
      }
      else if ((i < 3 && c == '.') || i == 3) {
        break;
      }
      else {
        return 0;
      }
    }
    if (n >= 256) {
      return 0;
    }
    v *= 256;
    v += n;
  }
  return v;
}


static char* int_to_ip(char *buffer, uint16_t ip){
  sprintf(buffer, "%d.%d.%d.%d",
      (ip >> 24) & 0xFF,
      (ip >> 16) & 0xFF,
      (ip >>  8) & 0xFF,
      (ip      ) & 0xFF);

  return buffer;
}


static bool _write_config_prop_str(const char *key, const char*str){
  fprintf(g_config->config_file, "%s=%s\n", key, str);
  return true;
}

static bool _write_config_prop_uint32(const char *key, uint32_t value){
  fprintf(g_config->config_file, "%s=%d\n", key, value);
  return true;
}

static bool _write_config_prop_uint16(const char *key, uint16_t value){
  fprintf(g_config->config_file, "%s=%d\n", key, value);
  return true;
}

static bool _write_config_prop_unsigned(const char *key, unsigned int value){
  fprintf(g_config->config_file, "%s=%d\n", key, value);
  return true;
}

static bool _write_config_prop_short(const char *key, short value){
  fprintf(g_config->config_file, "%s=%d\n", key, value);
  return true;
}

static bool _read_config_prop_uint32(const char *buf, uint32_t *value){
  char key[128];
  sscanf(buf, "%s=%d", key, value);
  return true;
}

static bool _read_config_prop_uint16(const char *buf, uint16_t *value){
  char key[128];
  sscanf(buf, "%s=%hu", key, value);
  return true;
}

static bool _read_config_prop_unsigned(const char *buf, unsigned int *value){
  char key[128];
  sscanf(buf, "%s=%u", key, value);
  return true;
}

static bool _read_config_prop_short(const char *buf, short *value){
  char key[128];
  sscanf(buf, "%s=%hu", key, value);
  return true;
}

static bool _read_config_prop_str(const char *buf, char *value){
  char key[128];
  sscanf(buf, "%s=%s", key, value);
  return true;
}

static bool has_prefix(const char*str, const char *pre) {
  size_t lenpre = strlen(pre),
         lenstr = strlen(str);
  return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

uint32_t config_local_ip_int32(){
  return ip_to_int(g_config->localIp);
}

uint32_t config_device_ip_int32(){
  return ip_to_int(g_config->deviceIp);
}
