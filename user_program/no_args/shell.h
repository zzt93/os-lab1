//
// Created by zzt on 11/15/16.
//

#ifndef OS_LAB1_SHELL_H
#define OS_LAB1_SHELL_H


#define BUF_SZ 256
// one for command/executable itself
#define MAX_PARAMETER_NR (10 + 1)
//#define NAME_LEN 32

const char const * CD = "cd";
const char const * PWD = "pwd";
//char *user_name[NAME_LEN] = "zzt@os-lab: ";

const char *simplify_path(const char *path);

#endif //OS_LAB1_SHELL_H
