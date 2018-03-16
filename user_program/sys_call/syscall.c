

int __attribute__((__noinline__))
syscall(int id, ...) {
    int ret;
    int *args = &id;
    /**
        return using $eax
        a -- $eax; b -- $ebx ; c -- $ecx; d -- $edx;
    */
    asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
    return ret;
}

