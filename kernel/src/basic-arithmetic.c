#include "basic/arithmetic.h"
#include "interpreter.h"

value_t val_add(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) + basic_getnum(rhs));
}

value_t val_sub(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) - basic_getnum(rhs));
}

value_t val_mul(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) * basic_getnum(rhs));
}

value_t val_div(value_t lhs, value_t rhs)
{
	number_t l = basic_getnum(lhs);
	number_t r = basic_getnum(rhs);
	if(r != 0) {
		return basic_mknum(l / r);
	} else {
		basic_error(ERR_DIV_BY_ZERO);
		return basic_mknull();
	}
}

value_t val_neg(value_t lhs)
{
	return basic_mknum(-basic_getnum(lhs));
}


value_t val_and(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) && basic_getnum(rhs));
}

value_t val_or(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) || basic_getnum(rhs));
}

value_t val_xor(value_t lhs, value_t rhs)
{
	return basic_mknum(!!basic_getnum(lhs) ^ !!basic_getnum(rhs));
}

value_t val_not(value_t lhs)
{
	return basic_mknum(!basic_getnum(lhs));
}