#include<lib.h>

int main(int argc, char **argv) {
    if (argc > 2) {
        user_panic("your args is too many!");
        return;
    }
    int r = create(argv[1], FTYPE_DIR);
    if (r < 0) {
        debugf("fail to mkdir: %d\n", r);
    }
    return 0;
}