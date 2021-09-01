#include <stdlib.h>
#include <unistd.h>

#include "ed.h"

static const char *defaultLocalIP = "192.1.1.1";
static const char *defaultDeviceIP = "192.168.1.41";

static int32_t ip_to_int(const char*);

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

  g_config->localIp = ip_to_int(defaultLocalIP);
  g_config->localPort = 5000;

  g_config->deviceIp = ip_to_int(defaultDeviceIP);
  g_config->devicePort = 5000;

  g_config->ad_channel = ADCHANNEL_SINGLE;
  g_config->ad_bit = ADBIT_12;
  g_config->trigger = TRIGGER_UP;
  g_config->outer_trigger = OUTER_TRIGGER_UP;

  // default set to stdout
  g_config->log_file = fdopen(STDOUT_FILENO, "a");

  return g_config;
}

static int32_t
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

