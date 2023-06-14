#include <lib.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        debugf("your argvs is too many!\n");
    }
    char t[1024];
    syscall_getworkdir(t);
    printf("%s\n", t);
    return 0;
}