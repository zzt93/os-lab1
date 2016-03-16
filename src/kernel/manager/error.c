#include "error.h"
#include "macro.h"

// the error must correspond to the order of err in message.h
// e.g. err[NO_SUCH] == "No such file or directory"
// @see set_error_msg(), message.h
const char *const err[] = {
    "Success",
    "Unknown/Unclassified error",
    // FM related error message
    "No such file or directory",
    "Is a directory",
    "File or directory already exist",
    "No more disk storage",
    "Parameter buffer is too small",
    "Invalid file name for this operation",
    "File error",
    // PM related error message
    "Segmentation fault",
    "Invalid priority for user process",
    "No such file or not executable for exec",
    "Process related error",
    // MM related error message
    "NO more memory to allocate",
    "Memory error",
};

const int len_err = ARR_LEN(err);
int err_size[ARR_LEN(err)];
