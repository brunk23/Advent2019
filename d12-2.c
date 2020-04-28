#include <u.h>
#include <libc.h>
#include <stdio.h>

#define MAXLIST 1000000

/*
 *  Need 64 bit numbers for the final answer.
 */

typedef struct Vector Vector;
typedef struct Planet Planet;

struct
Vector
{
	int x;
	int y;
	int z;
};

struct
Planet
{
	Vector pos;
	Vector vel;
};

vlong gcd(vlong, vlong);
void update_next( int r[] );
int any_match( int r[], int);
void p( int r[] );


void
main(int argc, char *argv[])
{
	FILE *fp;
	Planet planets[4];
	int x, y, z;
	vlong a, b, c, i = 0;
	int *repeat = nil;

	if( !( repeat = malloc( 8 * MAXLIST * sizeof(int) ) ) ) {
		exits("Failed to get array.");
	}

	if( !( fp=fopen("d12-input", "r") )) {
		exits("Can't open file.");
	}

	while( EOF != (fscanf(fp,"<x=%d, y=%d, z=%d>\n", &x, &y, &z))) {
		planets[i].pos.x = x;
		planets[i].pos.y = y;
		planets[i].pos.z = z;
		planets[i].vel.x = 0;
		planets[i].vel.y = 0;
		planets[i].vel.z = 0;
		i++;
	}

	for( i = 0; i < 4; i++ ) {
		repeat[i] = planets[i].pos.x;
		repeat[i+4] = planets[i].vel.x;
	}
	for( i = 0; i < MAXLIST - 1; i++ ) {
		update_next( &repeat[i*8] );
		if( any_match( repeat, (i+1)*8 ) ) {
			break;
		}
	}
	a = i+1;
	print("The x will repeat after %lld loops\n", i+1);

	for( i = 0; i < 4; i++ ) {
		repeat[i] = planets[i].pos.y;
		repeat[i+4] = planets[i].vel.y;
	}
	for( i = 0; i < MAXLIST - 1; i++ ) {
		update_next( &repeat[i*8] );
		if( any_match( repeat, (i+1)*8 ) ) {
			break;
		}
	}
	b = i+1;
	print("The y will repeat after %lld loops\n", i+1);

	for( i = 0; i < 4; i++ ) {
		repeat[i] = planets[i].pos.z;
		repeat[i+4] = planets[i].vel.z;
	}
	for( i = 0; i < MAXLIST - 1; i++ ) {
		update_next( &repeat[i*8] );
		if( any_match( repeat, (i+1)*8 ) ) {
			break;
		}
	}
	c = i+1;
	print("The z will repeat after %lld loops\n", i+1);

	c /= gcd( c, b);
	i = c * b;
	i /= gcd( i, a );
	i *= a;

	print("All the points will repeat every %lld iterations\n", i);

	free(repeat);
	fclose(fp);
	exits(0);
}

vlong
gcd(vlong x, vlong y)
{
	vlong z = 0;
	while( y ) {
		z = y;
		y = x % y;
		x = z;
	}
	return x;
}

/*
 * 0 through 3 are positions, 4 through 7 are velocities
 * 8 through 11 are new positions, 12 through 15 are new velocities
 */
void
update_next( int *r )
{
	int i, j;
	for( i = 0; i < 8; i++ ) {
		r[i + 8] = r[i];
	}
	for( i = 0; i < 3; i++ ) {
		for( j=i+1; j < 4; j++ ) {
			if( r[i] > r[j] ) {
				r[i+12] -= 1;
				r[j+12] += 1;
			} else {
				if( r[i] < r[j] ) {
					r[i+12] += 1;
					r[j+12] -= 1;
				}
			}
		}
	}
	for( i = 8; i < 12; i++ ) {
		r[i] += r[i+4];
	}
}

void
p( int *r )
{
	int i;
	for( i = 0; i < 8; i++ ) {
		print("%d, ", r[i]);
	}
	print("\n");
}

int
any_match( int r[], int max ) {
	int i, j;

	for( i = 0; i < max; i = i + 8 ) {
		if( r[i] != r[max] ) {
			continue;
		}
		for( j = 1; j < 8; j++ ) {
			if( r[ j + i ] != r[ max + j ] ) {
				break;
			}
		}
		if( j == 8 ) {
			print("States: %d and %d match!\n", i/8, max/8);
			return 1;
		}
	}
	return 0;
}
