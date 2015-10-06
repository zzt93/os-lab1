#include "error.h"
#include "macro.h"

// the error must correspond to the order of err in message.h
// e.g. err[- NO_SUCH] == "No such file or directory"
// @see set_error_msg(), message.h
const char *const err[] = {
    "Unknown/Unclassified error",
    "File error",
    "No such file or directory",
    "is a directory",
    "File or directory already exist",
    "No more disk storage",
    "Parameter buffer is too small",
    "Process related error",
    "Segmentation fault",
    "Memory error",
    "NO more memory to allocate",
};

const int len_err = ARR_LEN(err);
int err_size[ARR_LEN(err)];
