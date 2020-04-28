#include <u.h>
#include <libc.h>
#include <stdio.h>

#define HEIGHT 21
#define WIDTH 21
#define SIZE HEIGHT * WIDTH
#define MAXRISE HEIGHT + 1
#define MAXDIST (WIDTH + 1) * (WIDTH + 1) + (HEIGHT + 1) * (HEIGHT + 1)
#define X 11
#define Y 13

/*
 * (11,13) is the point we will work from.
 */

typedef struct Slope Slope;
typedef struct Location Location;

enum
{
	up = 0, right = 1, down = 2, left = -1
};

struct
Slope
{
	int rise;
	int run;
	int distance;
	int used;
	int x;
	int y;
} s[SIZE];

struct
Location
{
	int asteroid;
	int cansee;
};

Slope *find_next( Location map[HEIGHT][WIDTH], Slope *p, int run);
int compare_slope(Slope *a, Slope *b);
int gcd(int, int);
int how_many( Location map[][], int x, int y );
int verify( Slope *s, int run );
void run_laser( Location map[][] );
void insert(int, int, int, int);
void vaporize(Slope *, int);

void
vaporize(Slope *s, int i)
{
	if( i > 0 ) {
		print("The %d asteroid to be vaporized it at %d,%d\n", i, s->x, s->y );
	}
	s->used = 0;
}

int
compare_slope(Slope *a, Slope *b)
{
	int numa, numb;

	if( !b ) {
		return 1;
	}

	if( a->rise == b->rise && a->run == b->run ) {
		return 0;
	}

	numa = b->run * a->rise;
	numb = a->run * b->rise;

	if( numa > numb ) {
		return -1;
	}
	return 1;
}

void
insert(int rise, int run, int x, int y)
{
	int i, dist;

	if( !(rise || run) ) {
		return;
	}

	dist = (rise*rise)+(run*run);

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

	for( i = 0; i < SIZE; i++ ) {
		if( !s[i].used ) {
			s[i].rise = rise;
			s[i].run = run;
			s[i].distance = dist;
			s[i].used = 1;
			s[i].x = x;
			s[i].y = y;
			return;
		}
	}
}

int
how_many( Location map[HEIGHT][WIDTH], int x, int y )
{
	int a, b, rise, run;

	for( a = 0; a < SIZE; a++ ) {
		s[a].used = 0;
	}

	for( b = 0; b < HEIGHT; b++ ) {
		for( a = 0; a < WIDTH; a++ ) {
			if( map[b][a].asteroid ) {
				rise = b - y;
				run = a - x;
				insert(rise, run, a, b);
			}
		}
	}

	b = 0;

	for( a = 0; a < SIZE; a++ ) {
		if( s[a].used ) {
			b++;
		}
	}
	return b;
}

int
verify( Slope *s, int run )
{
	if( run == right ) {
		return ( s->run > 0 );
	}
	if( run == left ) {
		return ( s->run < 0 );
	}
	if( run == up ) {
		return ( (s->rise < 0) && (s->run == 0) );
	}
	return ( (s->rise) > 0 && (s->run == 0) );
}

Slope *
find_next( Location map[HEIGHT][WIDTH], Slope *p, int run)
{
	int x, y, distance;
	Slope *currptr;

	distance = MAXDIST;
	currptr = nil;

	for( x = 0; x < SIZE; x++ ) {
		if( s[x].used && verify( &s[x], run ) ) {
			if( (y = compare_slope( &s[x], currptr )) == 0 ) {
				if( s[x].distance < distance ) {
					currptr = &s[x];
					distance = s[x].distance;
				}
			} else {
				if( y == 1 ) {
					if( !p || (compare_slope( &s[x], p ) == -1) ) {
						currptr = &s[x];
						distance = s[x].distance;
					}
				}
			}
		}
	}
	return currptr;
}


/*
 * This will find each asteroid and count how many it can see.  Then destroys them.
 */
void
run_laser( Location map[HEIGHT][WIDTH] )
{
	int count = 0;
	Slope *currptr, *prevptr;

	map[Y][X].cansee = how_many( map, X, Y );

	while( map[Y][X].cansee ) {
		prevptr = nil;
		/* start by looking directly up */
		currptr = find_next(map, prevptr, up);
		if( currptr ) {
			count++;
			map[Y][X].cansee--;
			vaporize( currptr, count );
		}

		/* now we need to go through Q1 and Q4 */
		prevptr = nil;
		while( 1 ) {
			currptr = find_next(map, prevptr, right);
			if( currptr ) {
				count++;
				map[Y][X].cansee--;
				vaporize( currptr, count );
				prevptr = currptr;
			} else {
				break;
			}
		}

		prevptr = nil;
		/* next looking directly down */
		currptr = find_next(map, prevptr, down);
		if( currptr ) {
			count++;
			map[Y][X].cansee--;
			vaporize( currptr, count );
		}

		/* now we need to go through Q3 and Q2 */
		prevptr = nil;
		while( 1 ) {
			currptr = find_next(map, prevptr, left);
			if( currptr ) {
				count++;
				map[Y][X].cansee--;
				vaporize( currptr, count );
				prevptr = currptr;
			} else {
				break;
			}
		}
	}	
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

	for( y = 0; y < HEIGHT; y++ ) {
		len = fread( inp, sizeof(char), WIDTH+1, fp );
		if( len != (WIDTH+1) ) {
			print("Failed to get complete line. %d\n", len );
		}
		for( x = 0; x < WIDTH; x++ ) {
			map[y][x].cansee = 0;
			if( inp[x] == '#' ) {
				map[y][x].asteroid = 1;
			} else {
				map[y][x].asteroid = 0;
			}
		}
	}
	run_laser( map );

	fclose(fp);
	exits(0);
}
