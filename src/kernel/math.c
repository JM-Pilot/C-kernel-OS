#include <math.h>

int sqrt(int n) {
	float a = n;
	float b = 1;
	float e = 0.000001;
	while (a-b > e) {
		a = (a+b)/2;
		b = n/a;
	}
	return a;
}

char is_prime(int n) {
	if (n <= 1) return 0;
	for (int i = 2; i*i <= n; i++) {
		if (!(n % i)) return 0;
	}
	return 1;
}
