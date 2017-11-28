#pragma once

#include <map>
#include <string>

#define MAKE_FLAG_COLLECTION_BODY(FLAG_NAME) namespace FLAG_NAME {\
struct FLAG_NAME \
{ \
    static int minFlag, maxFlag;\
	static std::map<int, FLAG_NAME*> *flagNameTable; \
	FLAG_NAME(int value, const char *name) \
		: nValue(value), strName(name) \
	{ \
        static bool first = true; \
        if(first) {\
			minFlag = 2147483647; maxFlag = 0; first = false;\
		}\
        if(value > maxFlag) maxFlag = value; if(value < minFlag) minFlag = value;\
		TableInstance()->insert({value, this}); \
	} \
	const int nValue; \
	const char *strName; \
	static std::map<int, FLAG_NAME*> * TableInstance() { \
		static std::map<int, FLAG_NAME*> *ret = new std::map<int, FLAG_NAME*>();\
        flagNameTable = ret;\
		return ret;\
	}\
	static FLAG_NAME& FindFlag(int value) \
	{ \
		return *((*flagNameTable)[value]); \
	} \
}; \

#define FLAG_COLLECTION_BODY_END }

#define REGISTER_TABLE_NAME(FLAG_NAME) decltype(FLAG_NAME::FLAG_NAME::flagNameTable) FLAG_NAME::FLAG_NAME::flagNameTable;\
                                         int FLAG_NAME::FLAG_NAME::minFlag; int FLAG_NAME::FLAG_NAME::maxFlag;
