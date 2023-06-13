#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) { // 忽略空白符
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) { // 特殊符号 直接返回
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	if ('"' == *s) { // 支持双引号
		*p1 = ++s;
		while (*s != 0 && *s != '"') {
			s++;
		}
		if (*s == 0) user_panic("Expected another '\"' !\n");
		*s = 0;
		*p2 = ++s;
		return 'w';
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) { // s != NULL set np1, np2
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	/*
	s == NULL (0)  默认从读取词元的中断位置继续向后读
	这里的 c 和 nc 形成了一个微型缓冲区的结构
	返回值是一个词元，同时将 *p1 指向该 token
	*/
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

// int tokenpeek() {
// 	return 
// }

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) { // 超过最大参数个数 报错
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t; // 存下该参数的指针
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			/* Exercise 6.5: Your code here. (1/3) */
			r = open(t, O_RDONLY | O_CREAT); // 不存在时创建
			if (r < 0) {
				user_panic("The redirection path is not valid!\n");
			}
			dup(r, 0);
			close(r);
			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, dup it onto fd 1, and then close the original fd.
			/* Exercise 6.5: Your code here. (2/3) */
			r = open(t, O_WRONLY | O_CREAT); // 不存在时创建
			if (r < 0) {
				user_panic("The redirection path is not valid!\n");
			}
			dup(r, 1);
			close(r);
			break;
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			if ((r = pipe(p)) < 0) {
				user_panic("pipe alloc error %d!\n", r);
			}
			*rightpipe = fork();
			if (*rightpipe < 0) {
				user_panic("fork error %d!\n", *rightpipe);
			}
			if (*rightpipe == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe); // 子进程要继续解析 返回 | 右侧的解析结果
			}
			else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				return argc;
			}
			break;
		case ';': {
			int r;
			if ((r = fork()) < 0) { // fork一个子进程来执行
				user_panic("fork: %d", r);
			}
			if (r == 0) {
				return argc;
			}
			else {
				wait(r);
				return parsecmd(argv, rightpipe);
			}
		}
		case '&': {
			int r;
			if ((r = fork()) < 0) { // fork一个子进程来执行
				user_panic("fork: %d", r);
			}
			if (r == 0)
				return argc;
			else
				return parsecmd(argv, rightpipe); // 无阻塞直接运行
		}
		}
	}

	return argc;
}

void runcmd(char *s) {
	gettoken(s, 0); // set

	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		exit();
		return;
	}
	argv[argc] = 0;
	int u;
	// if (strcmp(argv[0], "cd") == 0) {
	// 	/*切换目录*/
	// 	return;
	// }
	// else {
	// 	u = fork();
	// }
	// if (u == 0) {
	int child = spawn(argv[0], argv);
	close_all();
	if (child >= 0) {
		wait(child);
	}
	else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit();// kill
	// for (int i = 0; i < argc; ++i)
	// 	debugf("%s ", argv[i]);
	// debugf("\n");
// }
}

#define BufLen 1024
struct Buf {
	char data;
	TAILQ_ENTRY(Buf) buf_link;
	int valid;
};
TAILQ_HEAD(Buf_list, Buf);
void initBuf(struct Buf *list, int n) {
	for (int i = 0;i < n;++i) {
		list[i].valid = 0;
		list[i].buf_link.tqe_next = NULL;
		list[i].buf_link.tqe_prev = NULL;
	}
}
struct Buf *allocBuf(struct Buf *list, int n) {
	for (int i = 0;i < n;++i) {
		if (list[i].valid == 0) {
			list[i].buf_link.tqe_next = NULL;
			list[i].buf_link.tqe_prev = NULL;
			list[i].valid = 1;
			return &list[i];
		}
	}
	user_panic("No spare buf left!");
	exit();
}
void freeBuflist(struct Buf_list *list) {
	struct Buf *cur;
	TAILQ_FOREACH(cur, list, buf_link)
		cur->valid = 0;
	TAILQ_INIT(list);
}
void readline(char *buf, u_int n) {
	// struct Buf *cursor = 0; // 当前 cursor 位置
	// int maxpos = 0; // 本行指令最大位置
	struct Buf node[BufLen];
	initBuf(node, BufLen);
	struct Buf_list frontbuf; // 光标位置前缓冲区
	struct Buf_list backbuf; // 光标位置后缓冲区
	TAILQ_INIT(&backbuf);
	TAILQ_INIT(&frontbuf);
	int r;
	for (;;) {
		char tempc;
		if ((r = read(0, &tempc, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		switch (tempc) {
		case 0x7f:
		case '\b':
			if (!TAILQ_EMPTY(&frontbuf)) {
				struct Buf *tlast = TAILQ_LAST(&frontbuf, Buf_list);
				tlast->valid = 0; // free
				TAILQ_REMOVE(&frontbuf, tlast, buf_link);
				printf("\x1b[1D\x1b[K");
				struct Buf *cur;
				int dis = 0;
				TAILQ_FOREACH(cur, &backbuf, buf_link) {
					printf("%c", cur->data);
					++dis;
				}
				while (dis--)
					printf("\b"); // 复原光标位置
			}
			break;
		case '\r':
		case '\n': {
			struct Buf *cur;
			int i = 0;
			TAILQ_FOREACH(cur, &frontbuf, buf_link)
				buf[i++] = cur->data;
			TAILQ_FOREACH(cur, &backbuf, buf_link)
				buf[i++] = cur->data;
			buf[i] = 0;
			return;
		}
		case 0x1B: { // 'ESC'
			char temp[4];
			if ((r = read(0, temp, 1)) != 1) {
				if (r < 0) {
					debugf("read error: %d\n", r);
				}
				exit();
			}
			if (temp[0] != '[') {
				debugf("cannot recognize\n");
				exit();
			}
			if ((r = read(0, temp + 1, 1)) != 1) {
				if (r < 0) {
					debugf("read error: %d\n", r);
				}
				exit();
			}
			switch (temp[1]) {
			case 'A': { // history prev
				printf("\x1b[B"); // 抵消
				char dst[1024];
				getPrev(dst);
				if (strlen(dst) == 0) break;
				struct Buf *cur;
				TAILQ_FOREACH(cur, &frontbuf, buf_link)
					printf("\x1b[1D");
				printf("\x1b[K");
				freeBuflist(&frontbuf);
				freeBuflist(&backbuf);
				for (int i = 0; dst[i] != '\0';++i) {
					struct Buf *tempnode = allocBuf(node, BufLen);
					tempnode->data = dst[i];
					TAILQ_INSERT_TAIL(&frontbuf, tempnode, buf_link);
				}
				printf("%s", dst);
				break;
			}
			case 'B': { // history next
				char dst[1024];
				getNxt(dst);
				if (strlen(dst) == 0) break;
				struct Buf *cur;
				TAILQ_FOREACH(cur, &frontbuf, buf_link)
					printf("\x1b[1D");
				printf("\x1b[K");
				freeBuflist(&frontbuf);
				freeBuflist(&backbuf);
				for (int i = 0; dst[i] != '\0';++i) {
					struct Buf *tempnode = allocBuf(node, BufLen);
					tempnode->data = dst[i];
					TAILQ_INSERT_TAIL(&frontbuf, tempnode, buf_link);
				}
				printf("%s", dst);
				break;
			}
			case 'D': // forward
				if (!TAILQ_EMPTY(&frontbuf)) {
					struct Buf *tlast = TAILQ_LAST(&frontbuf, Buf_list);
					TAILQ_REMOVE(&frontbuf, tlast, buf_link);
					TAILQ_INSERT_HEAD(&backbuf, tlast, buf_link);
					// printf("\b"); // 左移
				}
				else {
					printf("\x1b[1C"); // 抵消
				}
				break;
			case 'C': // backward
				if (!TAILQ_EMPTY(&backbuf)) {
					struct Buf *tfirst = TAILQ_FIRST(&backbuf);
					TAILQ_REMOVE(&backbuf, tfirst, buf_link);
					TAILQ_INSERT_TAIL(&frontbuf, tfirst, buf_link);
					// printf("\x1b[1C"); // 右移
				}
				else {
					printf("\b");// 抵消
				}
				break;
			}
			break;
		}
		default: {
			struct Buf *tempnode = allocBuf(node, BufLen);
			tempnode->data = tempc;
			TAILQ_INSERT_TAIL(&frontbuf, tempnode, buf_link);
			printf("\x1b[K"); // 清空光标后内容
			struct Buf *cur;
			int dis = 0;
			TAILQ_FOREACH(cur, &backbuf, buf_link) {
				printf("%c", cur->data);
				++dis;
			}
			while (dis--)
				printf("\b"); // 复原光标位置
		}
		}
	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	debugf("usage: sh [-dix] [command-file]\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0; // 回显命令
	debugf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	debugf("::                                                         ::\n");
	debugf("::                     MOS Shell 2023                      ::\n");
	debugf("::                                                         ::\n");
	debugf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN{
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
		ARGEND

		if (argc > 1) {
			usage();
		}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[1], r);
		}
		user_assert(r == 0);
	}
	history_init();
	for (;;) { // 编译出的汇编码可能会比while(1)少
		if (interactive) {
			printf("\n$ ");
		}
		readline(buf, sizeof buf);
		savecmd(buf);

		if (buf[0] == '#') { // 禁止根用户命令
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) { // 在子进程中执行命令
			runcmd(buf);
			exit();  // kill
		}
		else {
			wait(r);
		}
	}
	return 0;
}
