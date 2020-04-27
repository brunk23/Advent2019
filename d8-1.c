#include <u.h>
#include <libc.h>
#include <stdio.h>

#define WIDTH 25
#define HEIGHT 6
#define pagesize WIDTH * HEIGHT

void
main(int argc, char *argv[])
{
	FILE *fp = nil;
	char image[pagesize], final[pagesize];
	int i, count = 0, ones, twos;

	if( !( fp = fopen("d8-input", "r") ) ) {
		exits("Could not open file.");
	}

	for( i = 0; i < pagesize; i++ ) {
		final[i] = '2';
	}

	while( 150 == ( i = fread( image, sizeof(char), pagesize, fp ) ) ) {
		count = 0;
		ones = 0;
		twos = 0;
		for( i = 0; i < pagesize; i++ ) {
			if( final[i] == '2' ) {
				final[i] = image[i];
			}
			if( image[i] == '0' ) {
				count++;
				continue;
			}
			if( image[i] == '1' ) {
				ones++;
				continue;
			}
			twos++;
		}
		if( count == 8 ) {
			print("On the layer with 8 zeros, the product is %d\n", ones * twos);
		}
	}

	for( i = 0; i < pagesize; i++ ) {
		if( final[i] == '1' ) {
			print("#");
		} else {
			print(" ");
		}
		if( (i+1)%WIDTH == 0 ) {
			print("\n");
		}
	}

	fclose(fp);
}
