#pragma once

#include <map>
#include <string>

#define MAKE_FLAG_COLLECTION_BODY(FLAG_NAME) namespace FLAG_NAME {\
struct FLAG_NAME \
{ \
	FLAG_NAME(int value, const char *name) \
		: nValue(value), strName(name) \
	{ \
	} \
	const int nValue; \
	const char *strName; \
}; \

#define FLAG_COLLECTION_BODY_END }