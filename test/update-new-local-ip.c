#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ed.h"


int main(int argc, const char *argv[])
{
  config_t *c = (config_t *)malloc(sizeof(config_t));
  load_default_config(c);

  printf("default local port: %d\n", c->local_port);
  printf("default local ip: %s\n", c->local_ip);

  printf("default device port: %d\n", c->device_port);
  printf("default device ip: %s\n", c->device_ip);

  addr_t *a1 = (addr_t *)malloc(sizeof(addr_t));
  establish_connection(c,         a1);
  int r = connect_device(c, a1);
  if (r != CONNECT_SUCCESS) {
    exit(1);
  }

  puts("Input new local ip: ");
  scanf("%s", c->new_local_ip);
  puts("Input new local port: ");
  scanf("%d", &c->new_local_port);
  getchar();
  send_config_to_device(c, a1);
  teardown_connection(c, a1);

  return 0;
}

