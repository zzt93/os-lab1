#include "sys_call/io/io.h"
#include "shell.h"


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
    char cmd[ONE_CMD_MAX_LEN], copy[ONE_CMD_MAX_LEN];
    char *save[MAX_PARAMETER_NR] = {0};
    char *filename = NULL;
    int pid, count, res;
    while(1) {
        prompt();
        memset(cmd, 0, ONE_CMD_MAX_LEN);
        read_line(cmd, ONE_CMD_MAX_LEN);
        u_assert(cmd[0] != ' ', "should not start with space: %s\n", cmd);
        memcpy(copy, cmd, ONE_CMD_MAX_LEN);
        count = split(copy, ' ', save);
        u_assert(MAX_PARAMETER_NR >= count, "%d\n", count);
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
        if((pid = fork()) == 0) {// child process go this way
			// no thread will receive the response of exec, so it failure should be known by waitpid
            int res;
            if (count == 1) {
                // for example: `ls `
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
        else {// father wait here
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
