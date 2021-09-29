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

  addr_t *a = (addr_t *)malloc(sizeof(addr_t));
  puts("current device ip: ");
  scanf("%s", c->device_ip);
  getchar();

  puts("current device port: ");
  scanf("%d", &c->device_port);
  getchar();
  establish_connection(c, a);
  int r = connect_device(c, a);
  if (r != CONNECT_SUCCESS) {
    exit(1);
  }

  puts("input new device ip: ");
  scanf("%s", c->device_ip);
  getchar();

  puts("input new device port: ");
  scanf("%d", &c->device_port);
  getchar();

  FILE *new_device_ip_fp = fopen("/tmp/config_with_new_device_ip", "w");
  write_config(c, new_device_ip_fp);
  send_config_to_device(c, a);
  teardown_connection(c, a);
  fclose(new_device_ip_fp);

  FILE *new_device_ip_fp_for_read = fopen("/tmp/config_with_new_device_ip", "r");
  config_t *new_c = (config_t *)malloc(sizeof(config_t));
  addr_t *new_a = (addr_t *)malloc(sizeof(addr_t));
  load_config(new_c, new_device_ip_fp_for_read);

  establish_connection(new_c, new_a);
  int res = connect_device(new_c, new_a);
  if (res != CONNECT_SUCCESS) {
    exit(1);
  }
  teardown_connection(new_c, new_a);
  fclose(new_device_ip_fp_for_read);

  return 0;
}

