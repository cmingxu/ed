#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ed.h"

int pack_config(config_t *c, char *packed) {
  char code[8] = {0x44, 0xE1, 0xF4, 0x61, 0x43, 0xEF, 0xED, 0x65};
  char *pos = packed;
  memcpy(pos, code, 8);
  pos += 8;
  return 0;
}

int main(int argc, const char *argv[])
{
  config_t *c = create_default_config();
  assert(c->sampleCount == 100);

  disable_cache();
  assert(c->cache_enabled == false);

  enable_cache();
  assert(c->cache_enabled == true);

  printf("%d\n", c->localPort);
  printf("%d\n", c->localIp);

  printf("%d\n", c->localPort);
  printf("%d\n", c->localIp);
  printf ("'%s' is 0x%08x.\n", "192.1.1.1", c->localIp);


  char *holder = (char *)malloc(32);
  memset(holder, '\0', 32);
  pack_config(c, holder);
  for (int i = 0; i < 21; i++) {
    printf("%02x\n", holder[i] & 0xff);
  }


  return 0;
}
