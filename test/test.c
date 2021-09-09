#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ed.h"

int main(int argc, const char *argv[])
{
  config_t *c = (config_t *)malloc(sizeof(config_t));
  load_default_config(c);
  /*assert(c->sample_count == 1000000);*/

  disable_cache(c);
  assert(c->storage_enabled == false);

  enable_cache(c);
  assert(c->storage_enabled == true);

  printf("%d\n", c->local_port);
  printf("%s\n", c->local_ip);

  printf("%d\n", c->device_port);
  printf("%s\n", c->device_ip);

  FILE *fp = fopen("/tmp/ed_config", "w+");
  if (fp == NULL) {
    perror("fail open config");
  }
  write_config(c, fp);

  fclose(fp);
  FILE *fp1 = fopen("/tmp/ed_config", "rw");
  if (fp1 == NULL) {
    perror("fail open config");
  }
  config_t *c1 = (config_t *)malloc(sizeof(config_t));
  /*load_config(c1, fp1);*/

  load_default_config(c1);
  printf("%d\n", package_count(c1));
  /*assert(package_count(c1) == 4077);*/

  fclose(fp1);

  addr_t *a1 = (addr_t *)malloc(sizeof(addr_t));
  memset(a1, '\0', sizeof(addr_t));
  printf("%lu\n", sizeof(addr_t));
  int r = connect_device(c1, a1);
  printf("%d\n", r);
  if (r != CONNECT_SUCCESS) {
    exit(1);
  }
  stop_collect(c1, a1);

  send_config_to_device(c1, a1);
  enable_cache(c1);
  start_collect(c1, a1);
  FILE *result = fopen("/tmp/ed_storage", "w+");
  if (fp == NULL) {
    perror("fail open config");
  }
  start_recv(c1, a1, result);

  stop_collect(c1, a1);
  disconnect_device(c1, a1);

  return 0;
}
