#pragma once

#include "hal.h"

#define debug(a...) hal_debug(a)

void hexdump(char const * name, void const * mem, int size);