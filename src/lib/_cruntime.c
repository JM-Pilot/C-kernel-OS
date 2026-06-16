#include <stdint.h>
uint64_t __udivdi3(uint64_t n, uint64_t m) {
	uint64_t q = 0;
	uint64_t r = 0;
	for (int i = 63; i >= 0; i--) {
		r <<= 1;
		r |= (n >> i) & 1ULL;
		if (r >= m) {
			r -= m;
			q |= (1ULL << i);
		}
	}
	return q;
}
uint64_t __umoddi3(uint64_t n, uint64_t m) {
	uint64_t r = 0;
	for (int i = 63; i >= 0; i--) {
		r <<= 1;
		r |= (n >> i) & 1ULL;
		if (r >= m) r -= m;
	}
	return r;
}
uint64_t __divdi3(long long a, long long b) {
	int neg = 0;
	if (a < 0) {
		a = -a; neg ^= 1;
	}
	if (b < 0) {
		b = -b; neg ^= 1;
	}
	uint64_t res = __udivdi3((uint64_t)a, (uint64_t)b);
	return neg ? (uint64_t)(-(long long)res) : res;
}
uint64_t __muldi3(uint64_t a, uint64_t b) {
	uint64_t res = 0;
	while (b) {
		if (b & 1) res += a;
		a <<= 1;
		b >>= 1;
	}
	return res;
}
