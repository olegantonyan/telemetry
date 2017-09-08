#include "cc1101/cc1101.h"

static CC1101_t config;

bool cc1101_init(const CC1101_t *c) {
  config = *c;

  return true;
}
