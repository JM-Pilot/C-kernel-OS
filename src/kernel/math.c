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
	if (n == 2) return 1;
	if (!(n % 2)) return 0;
	int max = sqrt(n);
	for (int i = 3; i <= max; i += 2) {
		if (!(n % i)) return 0;
	}
	return 1;
}
