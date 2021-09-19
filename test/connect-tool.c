#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NEW_LOCAL_IP 1
#include "ed.h"

int main(int argc, const char *argv[])
{
  config_t *c = (config_t *)malloc(sizeof(config_t));
  load_default_config(c);

  printf("default local port: %d\n", c->local_port);
  printf("default local ip: %s\n", c->local_ip);
  printf("default device port: %d\n", c->device_port);
  printf("default device ip: %s\n", c->device_ip);

  puts("input device ip");
  scanf("%s", c->device_ip);
  getchar();

  puts("input device port");
  scanf("%d", &c->device_port);
  getchar();

  puts("input local ip");
  scanf("%s", c->local_ip);
  getchar();

  puts("input local port");
  scanf("%d", &c->local_port);
  getchar();

  puts("input new local ip");
  scanf("%s", c->new_local_ip);
  getchar();

  puts("input new local port");
  scanf("%d", &c->new_local_port);
  getchar();

  addr_t *a = (addr_t *)malloc(sizeof(addr_t));
  int r = connect_device(c, a);
  if (r != CONNECT_SUCCESS) {
    exit(1);
  }

  disconnect_device(c, a);

  return 0;
}


