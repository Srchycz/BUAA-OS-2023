#include <stdio.h>

int judge(int x) {
	if (x == 0) return 1;
	int len = 0;
	int bit[10];
	while (x) {
		bit[++len] = x % 10;
		x /= 10;
	}
	int s = 1;
	while (len >= s){
		if (bit[len] != bit[s]) {
			return 0;
		}
		--len;
		++s;
	}
	return 1;
}

int main() {
	int n;
	scanf("%d", &n);

	if (judge(n) > 0) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
