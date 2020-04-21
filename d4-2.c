#include <u.h>
#include <libc.h>

int passes(int number) {
	int curr = 0, prev = 10;

	int count[10] = {0,0,0,0,0,0,0,0,0,0};

	while( number > 0 ) {
		curr = number%10;
		number /= 10;

		if( prev < curr ) {
			return 0;
		}

		count[curr]++;
		prev = curr;
	}
	for( curr = 0; curr < 10; curr++ ) {
		if( count[curr] == 2 ) {
			return 1;
		}
	}
	return 0;
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
