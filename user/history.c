#include <lib.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        debugf("your argv is too many!\n");
        return 0;
    }
    int fd = open("/.history", O_RDONLY);
    if (fd < 0) {
        user_panic("history open fail: %d\n", fd);
    }
    char tempc;
    int r;
    int cnt = 1, flg = 0;
    while ((r = read(fd, &tempc, 1)) == 1) {
        if (tempc == '\n') {
            printf("\n");
            ++cnt;
            flg = 0;
        }
        else {
            if (flg == 0) {
                printf("%5d  ", cnt);
                flg = 1;
            }
            printf("%c", tempc);
        }
    }
    return 0;
}