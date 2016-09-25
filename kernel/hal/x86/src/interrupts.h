#pragma once

#include "cpu.h"

typedef struct cpu * (*interrupt_f)(struct cpu *);

extern interrupt_f interrupts[];