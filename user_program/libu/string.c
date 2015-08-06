
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
    char c = 0;
    while(*str != '\0') {
        if (*str == delimiter && c != delimiter) {
            *saveptr = str;
        }
        c = *str;
        str++;
    }
}

