#include <u.h>
#include <libc.h>

int passes(int number) {
	int curr = 0, prev = 10, match = 0;

	while( number > 0 ) {
		curr = number%10;
		number /= 10;

		if( prev < curr ) {
			return 0;
		}
		if( prev == curr ) {
			match = 1;
		}
		prev = curr;
	}
	return match;
}

int main(int argc, char *argv[]) {
	int min = 136818, max = 685979, curr = 0, count = 0;

	for( curr = min; curr <= max; curr++ ) {
		if( passes(curr) ) {
			count++;
		}
	}
	print("Number of passes: %d\n", count);

	return 0;
}
