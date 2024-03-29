#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "ed.h"
#include "utils.h"

int main(int argc, const char *argv[])
{
  config_t *cfg = (config_t *)malloc(sizeof(config_t));
  load_default_config(cfg);
  cfg->sample_count = 880000;
  cfg->delay_count = 453333;
  cfg->repeat_count = 30;

  // 连接设备
  addr_t *addr = (addr_t *)malloc(sizeof(addr_t));
  memset(addr, '\0', sizeof(addr_t));
  int establish_res;
  if((establish_res = establish_connection(cfg, addr)) != 0) {
    printf("establish_connection failed %d\n", establish_res);
    exit(1);
  }

  // 停止接受, 防止之前有采集任务, 可多次调用
  int stop_res = stop_collect(cfg, addr);
  if(stop_res != STOP_COLLECT_SUCCESS) {
    exit(1);
  }

  // 建立连接， 向设备发连接指令
  int connect_res = connect_device(cfg, addr);
  if (connect_res != CONNECT_SUCCESS) {
    exit(1);
  }

  // 发送配置到设备
  send_config_to_device(cfg, addr);

  // 准备接收数据的内存
  printf("bytes_count(c) is %d\n", bytes_count(cfg));
  void *buf = malloc(bytes_count(cfg));

  // 通知设备开始采集
  start_collect(cfg, addr);
  for(unsigned int i = 0; i < cfg->repeat_count; i++) {
	repeat_response_t *resp = (repeat_response_t *)malloc(sizeof(repeat_response_t));
	void *p = (void *)malloc(repeat_bytes_count_with_heading(cfg));
	if(!p) {
		perror("1123");
		exit(1);
	}
	resp->data = p;
	resp->data_size = repeat_bytes_count_with_heading(cfg);
  	size_t nread = start_recv_by_repeat(cfg, addr, resp, i, 500);
  	printf("received bytes count %ld\n", resp->recv_data_size);
  	printf("received packet count %ld\n", resp->recv_packet_count);
  	printf("expect packet count %ld\n", resp->packet_count);
  	_debug_hex(resp->data, 16);
	free(resp->data);
	free(resp);
  }

  printf("repeat_bytes_count %d\n", repeat_bytes_count_with_heading(cfg));
  printf("sample count %d\n", cfg->sample_count);
  printf("repeat count %d\n", cfg->repeat_count);

  // 清理环境/断开连接
  free(buf);
  teardown_connection(cfg, addr);

  return 0;
}
