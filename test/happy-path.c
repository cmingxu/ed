#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ed.h"

int main(int argc, const char *argv[])
{
  config_t *c = (config_t *)malloc(sizeof(config_t));
  load_default_config(c);
  assert(c->sample_count == 1000000);

  disable_cache(c);
  assert(!c->storage_enabled);

  enable_cache(c);
  assert(c->storage_enabled);

  printf("%d\n", c->local_port);
  printf("%s\n", c->local_ip);

  printf("%d\n", c->device_port);
  printf("%s\n", c->device_ip);

  config_t *c1 = (config_t *)malloc(sizeof(config_t));
  load_default_config(c1);
#ifdef CONFIG_SERIALIZE
  // 写入配置
  FILE *fp = fopen("/tmp/ed_config", "w+");
  if (fp == NULL) {
    perror("fail open config");
  }
  write_config(c, fp);
  fclose(fp);

  // 加载配置
  FILE *fp1 = fopen("/tmp/ed_config", "rw");
  if (fp1 == NULL) {
    perror("fail open config");
  }
  load_config(c1, fp1);

  fclose(fp1);
#endif

  // 连接设备
  addr_t *a1 = (addr_t *)malloc(sizeof(addr_t));
  memset(a1, '\0', sizeof(addr_t));
  printf("%lu\n", sizeof(addr_t));

  int establish_res;
  if((establish_res = establish_connection(c1, a1)) != 0) {
    printf("establish_connection failed %d\n", establish_res);
    exit(1);
  }
  // 停止接受, 防止之前有采集任务, 可多次调用
  stop_collect(c1, a1);

  int r = connect_device(c1, a1);
  if (r != CONNECT_SUCCESS) {
    exit(1);
  }

  // 停止接受, 防止之前有采集任务, 可多次调用
  /*stop_collect(c1, a1);*/

  // 发送配置到设备
  send_config_to_device(c1, a1);

  // 开启数据采集到文件标志
  /*enable_cache(c1);*/

  // 通知设备开始采集
  start_collect(c1, a1);

  printf("bytes_count(c) is %d\n", bytes_count(c));
  void *buf = malloc(bytes_count(c));
  size_t nread = start_recv(c1, a1, buf, bytes_count(c));
  printf("received bytes count %ld\n", nread);

  // 停止采集
  /*stop_collect(c1, a1);*/
  free(buf);

  // 断开连接
  teardown_connection(c1, a1);

  return 0;
}
