#include <blib.h>

size_t strlen(const char *s) {
	int res = 0;
	while (*s){
		s++;
		res++;
	}
	return res;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while (*dst) {
		dst++;
	}
	while (*src){
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	char *dest = (char *)s;
	char cons = c;
	for (int i = 0; i < n; i++) {
		dest[i] = cons;
	}
	return s;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	char *str1 = (char *)s1;
	char *str2 = (char *)s2;
	while (n--){
		if (*str1 != *str2) {
			return *str1 - *str2;
		}
		str1++;
		str2++;
	}
	return 0;
}
