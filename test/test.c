#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ed.h"

int main(int argc, const char *argv[])
{
  config_t *c = create_default_config();
  assert(c->sampleCount == 1000000);

  disable_cache();
  assert(c->cache_enabled == false);

  enable_cache();
  assert(c->cache_enabled == true);

  printf("%d\n", c->localPort);
  printf("%s\n", c->localIp);

  printf("%d\n", c->localPort);
  printf("%s\n", c->localIp);

  FILE *fp = fopen("/tmp/ed_config", "w+");
  if (fp == NULL) {
    perror("fail open config");
  }
  update_config_file(fp);
  write_config();
  read_config();

  assert(package_count() == 4077);

  fclose(fp);
  connect_device();

  return 0;
}
