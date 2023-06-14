#include <lib.h>

// Overview:
// change workdir to path
void chdir(int envid, char *path) {
    char now[1024];
    syscall_getworkdir(now);
    mergepath(now, path);
    struct Stat st;
    int r;
    if ((r = stat(path, &st)) < 0) {
        debugf("chdir error:%d\n", r);
        return;
    }
    if (!st.st_isdir) {
        debugf("%s is not a directory!\n", now);
        return;
    }
    syscall_chworkdir(envid, now);
}

// Overview:
// Merge the suffix on the path 
void mergepath(char *path, char *suffix) {
    if (suffix[0] == '/') {
        path[0] = '/';
        path[1] = '\0';
    }
    if (suffix[0] == '.' && suffix[1] == '/') // 略去当前目录的表述
        suffix += 2;
    int len = strlen(path);
    if (path[len - 1] != '/') path[len++] = '/';
    int i = 0;
    for (;suffix[i] != '\0'; ++i) {
        if (suffix[i] == '.') {
            if (suffix[i + 1] == '.') {
                cdup(path, len);
                len = strlen(path);
                ++i;
                continue;
            }
            else if (suffix[i + 1] == '/') {
                ++i;
                continue;
            }
        }
        if (suffix[i] == '/' && path[len - 1] == '/') {
            continue;
        }
        path[len++] = suffix[i];
    }
    path[len] = '\0';
}

// Overview:
// Set path to backward level
void cdup(char *path, int len) {
    if (len <= 0) return;
    if (path[len - 1] == '/') --len;
    while (len > 0 && path[len - 1] != '/')
        --len;
    path[len] = '\0';
}