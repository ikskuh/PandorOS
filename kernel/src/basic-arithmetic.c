#include "basic/arithmetic.h"
#include "interpreter.h"
#include "standard.h"
#include "hal.h"

value_t val_add(value_t lhs, value_t rhs)
{
	if(lhs.type == TYPE_TEXT || rhs.type == TYPE_TEXT) {
		char const *left, *right;
		switch(lhs.type)
		{
			case TYPE_TEXT: left = basic_getstr(lhs); break;
			case TYPE_NUM: {
				left = basic_alloc(TYPE_NUM_LEN);
				int_to_string((char*)left, basic_getnum(lhs), 10);
				break;
			}
			default: basic_error(ERR_INVALID_TYPE); break;
		}
		switch(rhs.type)
		{
			case TYPE_TEXT: right = basic_getstr(rhs); break;
			case TYPE_NUM: {
				right = basic_alloc(TYPE_NUM_LEN);
				int_to_string((char*)right, basic_getnum(rhs), 10);
				break;
			}
			default: basic_error(ERR_INVALID_TYPE); break;
		}
		
		int leftlen = str_len(left);
		int totlen = leftlen + str_len(right) + 1;
		
		hal_debug("'%s' + '%s'\n", left, right);
		
		char *result = basic_alloc(totlen);
		str_copy(result, left);
		str_copy(result + leftlen, right);
		
		return basic_mkstr(result);
		
	} else {
		return basic_mknum(basic_getnum(lhs) + basic_getnum(rhs));
	}
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


value_t val_eq(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) == basic_getnum(rhs));
}
value_t val_neq(value_t lhs, value_t rhs)
{
	return basic_mknum(!basic_getnum(val_eq(lhs, rhs)));
}


value_t val_lt(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) < basic_getnum(rhs));
}
value_t val_le(value_t lhs, value_t rhs)
{
	return basic_mknum(basic_getnum(lhs) <= basic_getnum(rhs));
}