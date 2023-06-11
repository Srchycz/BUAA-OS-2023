#include <lib.h>

int flag[256];

void _tree(char *, int);

void tree(char *path) {
    int r;
    struct Stat st;

    if ((r = stat(path, &st)) < 0) {
        user_panic("stat %s: %d", path, r);
    }
    if (!st.st_isdir) {
        user_panic("The path is not a directory!\n");
    }
    _tree(path, 0);
}

void _tree(char *path, int dep) {
    // debugf("path:%s len:%d dep:%d\n", path, strlen(path), dep);
    int fd, n;
    struct File f;

    if ((fd = open(path, O_RDONLY)) < 0) {
        user_panic("open %s: %d", path, fd);
    }
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
        if (f.f_name[0]) {
            int i;
            for (i = 0;i < dep; ++i)
                printf("| ");
            printf("|--%s\n", f.f_name);
            if (f.f_type == FTYPE_DIR) {
                char newpath[1024];
                int i = 0, j = 0;
                // debugf("oldpath:%s\n", path);
                while (path[i] != '\0') {
                    newpath[i] = path[i];
                    ++i;
                }
                newpath[i++] = '/';
                while (f.f_name[j] != '\0')
                    newpath[i++] = f.f_name[j++];
                newpath[i] = '\0';
                // for (int k = 0; k <= i; ++k) {
                //     debugf("%d", newpath[k]);
                // }
                // debugf("\n");
                // debugf("%s\n", newpath);
                _tree(newpath, dep + 1);
            }
        }
    }
    if (n > 0) {
        user_panic("short read in directory %s", path);
    }
    if (n < 0) {
        user_panic("error reading directory %s: %d", path, n);
    }
}

int main(int argc, char **argv) {
    int i;

    if (argc < 1) {
        tree("/");
    }
    else
        tree(argv[1]);
    return 0;
}