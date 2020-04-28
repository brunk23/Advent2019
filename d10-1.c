#include <u.h>
#include <libc.h>
#include <stdio.h>

#define HEIGHT 21
#define WIDTH 21

/*
 * This idea here is to draw lines from each point through all the edge points.
 */

typedef struct Slope Slope;
typedef struct Location Location;

struct
Slope
{
	int rise;
	int run;
	int used;
} s[HEIGHT*WIDTH];

struct
Location
{
	int asteroid;
	int cansee;
};

int gcd(int, int);
void find_counts( Location map[][] );
int howmany( Location map[][], int x, int y );
void insert(int, int);

void
insert(int rise, int run)
{
	int i;

	if( !(rise || run) ) {
		return;
	}

	if( rise == 0 ) {
		if( run > 0 ) {
			run = 1;
		} else {
			run = -1;
		}
	}

	if( run == 0 ) {
		if( rise > 0 ) {
			rise = 1;
		} else {
			rise = -1;
		}
	}

	i = gcd( rise, run );
	rise /= i;
	run /= i;

	for( i = 0; i < (HEIGHT * WIDTH); i++ ) {
		if( !s[i].used ) {
			s[i].rise = rise;
			s[i].run = run;
			s[i].used = 1;
			return;
		}
		if( s[i].rise == rise && s[i].run == run ) {
			return;
		}
	}
}

int
howmany( Location map[HEIGHT][WIDTH], int x, int y )
{
	int a, b, rise, run;

	for( a = 0; a < (HEIGHT * WIDTH); a++ ) {
		s[a].used = 0;
	}

	for( b = 0; b < HEIGHT; b++ ) {
		for( a = 0; a < WIDTH; a++ ) {
			if( map[b][a].asteroid ) {
				rise = b - y;
				run = a - x;
				insert(rise, run);
			}
		}
	}

	b = 0;

	for( a = 0; a < (HEIGHT * WIDTH); a++ ) {
		if( s[a].used ) {
			b++;
		}
	}
	return b;
}

/*
 * This will find each asteroid and count how many it can see.
 */
void
find_counts( Location map[HEIGHT][WIDTH] )
{
	int x, y, max = 0;

	for( y = 0; y < HEIGHT; y++ ) {
		for( x = 0; x < WIDTH; x++ ) {
			if( map[y][x].asteroid ) {
				map[y][x].cansee = howmany( map, x, y );
				if( map[y][x].cansee >= max ) {
					max = map[y][x].cansee;
					print("(%d,%d) - %d\n", x, y, map[y][x].cansee );
				}
			}
		}
	}
	print("Max: %d\n", max);
}

int
gcd(int a, int b)
{
	int x = abs(a), y = abs(b), z = 0;

	if( x > 0 && y > 0 ) {
		while( y ) {
			z = y;
			y = x % y;
			x = z;
		}
		return x;
	}
	return 1;
}

void
main(int argc, char *argv[])
{
	FILE *fp = nil;
	Location map[HEIGHT][WIDTH];
	char inp[WIDTH+1];
	int x, y, len;

	if( !( fp = fopen("d10-input", "r") ) ) {
		exits("Could not open file.");
	}

	for( x = 0; x < HEIGHT; x++ ) {
		len = fread( inp, sizeof(char), WIDTH+1, fp );
		if( len != (WIDTH+1) ) {
			print("Failed to get complete line. %d\n", len );
		}
		for( y = 0; y < WIDTH; y++ ) {
			map[x][y].cansee = 0;
			if( inp[y] == '#' ) {
				map[x][y].asteroid = 1;
			} else {
				map[x][y].asteroid = 0;
			}
		}
	}
	find_counts( map );

	fclose(fp);
	exits(0);
}
