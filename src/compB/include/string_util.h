#ifndef STRING_UTIL_H_INCLUDED
#define STRING_UTIL_H_INCLUDED

#include "include_util.h"

void copyString(char *buf, char *string, ssize_t buf_size);

void copyByte(void *buf, void *string, size_t string_size);

int cmpByte(void *buf1, void *buf2, size_t buf1_size);

int str2num(char *target_string);

char* num2str(int target_num);

#endif
