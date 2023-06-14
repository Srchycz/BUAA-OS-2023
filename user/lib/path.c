#include <lib.h>

// Overview:
// Merge the suffix on the path 
void mergepath(char *path, char *suffix) {
    if (suffix[0] == '.' && suffix[1] == '/') // 略去当前目录的表述
        suffix += 2;
    int len = strlen(path);
    if (path[len] != '/') path[len++] = '/';
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