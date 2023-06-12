#include <lib.h>

int isUpdate;

// Overview:
// create and initialize .history
void history_init(void) {
    int r = create("/.history", FTYPE_REG);
    if (r < 0) {
        user_panic("history_init fail: %d\n", r);
    }
    isUpdate = 1;
}

// Overview:
// append buf of cmd to .history
// if strlen(buf) == 0, it will be ignored
void savecmd(char *buf) {
    int r = open("/.history", O_WRONLY);
    if (r < 0) {
        user_panic("history open fail: %d\n", r);
    }
    if (strlen(buf) != 0) {
        write(r, buf, strlen(buf));
        write(r, "\n", 1);
    }
    isUpdate = 1; // .history is updated
}

// Overview:
// Save the previous instruction to dst from .history
void getPrev(char *dst) {
    getInstr(dst, H_FORWARD);
}

// Overview:
// Save the next instruction to dst from .history
void getNxt(char *dst) {
    getInstr(dst, H_BACKWARD);
}

void getInstr(char *dst, int direct) {
    static struct Fd *fd;
    static int fdnum;
    static char *Lva;
    static char *Rva;
    static char *l;
    static char *r;
    if (isUpdate) {
        fdnum = open("/.history", O_RDONLY);
        if (fdnum < 0) {
            user_panic("fail to open /.history: %d\n", fdnum);
        }
        fd = num2fd(fdnum);
        Lva = fd2data(fd);
        Rva = Lva + ((struct Filefd *)fd)->f_file.f_size;
        l = r = Rva - 1;
    }
    if (direct == H_FORWARD) {
        if (l >= Lva && *l == '\n') {
            r = l;
            --l;
        }
        while (l >= Lva && *l != '\n') --l; // l被设为上一条指令的起始位置 - 1
    }
    else {
        if (r < Rva && *r == '\n') {
            l = r;
            ++r;
        }
        while (r < Rva && *r != '\n') ++r;
    }
    for (int i = 0; i < r - l - 1; ++i) {
        dst[i] = l[i + 1];
    }
    dst[r - l - 1] = '\0';
    isUpdate = 0;
}