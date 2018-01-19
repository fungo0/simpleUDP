#include "include/string_util.h"
#include "include/include_util.h"

void copyString(char *buf, char *string, ssize_t buf_size) {
    strncpy(buf, string, buf_size);
    buf[strlen(string)+1] = '\0';
    return;
}

void copyByte(void *buf, void *string, size_t string_size) {
    memcpy(buf, string, string_size);
    return;
}

int cmpByte(void *buf1, void *buf2, size_t buf1_size) {
    return memcmp(buf1, buf2, buf1_size);
}

int str2num(char *target_string) {
    int num;

    num = (int) strtol(target_string, (char **)NULL, 10);

    return num;
}

char* num2str(int target_num) {
    int length = snprintf(NULL, 0, "%d", target_num);
    char* str = malloc(length+1);
    memset(str, 0, length+1);
    snprintf(str, length+1, "%d", target_num);

    return str;
}
