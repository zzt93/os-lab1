

/**
   - This function is thread-safe
   - no invalid character check
   str -- just a plain decimal number, hex, octal is not allowed
 */
int to_int(char *str) {
    int res = 0;
    while (*str != '\0') {
        res *= 10;
        res += *str - '0';
        str++;
    }
    return res;
}

/**
   - This function is thread safe
   - Usage: @see strtok();
 */
void split(char *str, char delimiter, char **saveptr) {
    // skip heading delimiter
    while (*str != '\0' && *str == delimiter) {
        str++;
    }
    if (*str == '\0') {
        return;
    }

    // save the first part
    *saveptr ++ = str;
    char last_c = 0;
    while(*str != '\0') {
        if (*str != delimiter && last_c == delimiter) {
            *saveptr ++ = str;
        }
        last_c = *str;
        str++;
    }
}

