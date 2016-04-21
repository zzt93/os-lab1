#include "kernel/syscall.h"
#include "sys_call/io/io.h"
#include "lib/string.h"
#include "error.h"


#define BUF_SZ 256
// one for command itself
#define MAX_PARAMETER_NR (10 + 1)
//#define NAME_LEN 32

const char const * CD = "cd";
const char const * PWD = "pwd";
//char *user_name[NAME_LEN] = "zzt@os-lab: ";

bool check_args_num(int real, int target) {
    if (real == target) {
        return 1;
    } else if (real > target) {
        puts("too many arguments");
        return 0;
    }
    puts("too few arguments");
    return 0;

}

int entry() {
    char cmd[BUF_SZ], copy[BUF_SZ];
    char *save[MAX_PARAMETER_NR] = {0};
    char *filename = NULL;
    int pid, count, res;
    while(1) {
        prompt();
        memset(cmd, 0, BUF_SZ);
        read_line(cmd, BUF_SZ);
        memcpy(copy, cmd, BUF_SZ);
        count = split(copy, ' ', save);
        user_assert(MAX_PARAMETER_NR >= count);
        if (count < 1) {
            printf("Unknown command: %s\n", cmd);
            continue;
        }
        // check shell built-in command
        if (strcmp(save[0], CD) == 0)  {
            if (!check_args_num(count, 2)) {
                continue;
            }
            int res = chdir(save[1]);
            if (res != SUCC) {
                printf("%s: %s\n", get_err_msg(res), save[1]);
            }
            continue;
        } else if (strcmp(save[0], PWD) == 0) {
            if (!check_args_num(count, 1)) {
                continue;
            }
            print_cwd_path();
            continue;
        }
        filename = save[0];

        // TODO check file existence and whether it is executable -- many be checked by exec
		// TODO add redirect: <, >, 2>, &>, >>
        // TODO add pipe
        if((pid = fork()) == 0) {
			// no thread will receive the response of exec, so it failure should be known by waitpid
            int res;
            if (count == 1) {
                res = exec(filename, "");
            } else {
                res = exec(filename, save[1] - copy + cmd);
            }
            // if run to here, means fail to replace
            // the forked process, so just remind.
            printf("No such file or not executable: %s -- %s",
                filename,
                get_err_msg(res));
        }
        else {
			// if exec fail, waitpid will fail
            res = waitpid(pid);
            if (res != SUCC) {
                printf("Unknown command or wrong args: %s\n", cmd);
                continue;
            }

        }
    }
    return 0;
}
