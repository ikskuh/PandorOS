#pragma once

#include "types.h"

value_t val_add(value_t lhs, value_t rhs);

value_t val_sub(value_t lhs, value_t rhs);

value_t val_mul(value_t lhs, value_t rhs);

value_t val_div(value_t lhs, value_t rhs);

value_t val_neg(value_t lhs);


value_t val_and(value_t lhs, value_t rhs);

value_t val_or(value_t lhs, value_t rhs);

value_t val_xor(value_t lhs, value_t rhs);

value_t val_not(value_t lhs);

value_t val_eq(value_t lhs, value_t rhs);
value_t val_neq(value_t lhs, value_t rhs);
value_t val_lt(value_t lhs, value_t rhs);
value_t val_le(value_t lhs, value_t rhs);