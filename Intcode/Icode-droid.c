#include <u.h>
#include <libc.h>
#include "machine.h"

#define MAXDIST 99999

void create_map(void);
void resize_map(int, int, int, int);
void print_map(void);
void init_droid(void);
void fill(int, int, int);
void setdist(int, int, int);
void visit(int, int);
int val(int, int);
int dist(int, int);
int visited(int, int);
void reset_dist(void);

typedef struct droid droid;
typedef struct Map Map;
typedef struct Loc Loc;

enum
{
	north = 1, south, west, east
};

enum
{
	unknown = '?', wall = '#', known = '.', start = '*', goal = '$'
};

enum
{
	WALL = 0, MOVE, AIR
};

struct droid
{
	int dir;
	int x;
	int y;
} D;

struct Loc
{
	int dist;
	int val;
	int visited;
};

struct Map
{
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	Loc *v;
} flr;

struct ioqueue
{
	int cansend;
	int canread;
	vlong val;
} io;

int maxdist;
int foundo;
droid G;

void
main(int argc, char *argv[])
{
	Intcode *M = nil;

	if( !( M = malloc( sizeof(Intcode) ) ) ) {
		exits("Could not create machine.");
	}

	maxdist = MAXDIST;
	foundo = 0;
	G.x = 0;
	G.y = 0;

	init(M);
	populate(M);

	init_droid();
	create_map();

	io.cansend = 0;
	io.canread = 1;
	M->state = RUNNING;
	
	while( M->state == RUNNING ) {
		step(M);
	}

	print_map();
	print("Goal at %dx%d and %d steps.\n", G.x, G.y, dist(G.x, G.y) );

	exits(0);
}

void
icm_in(Intcode *M)
{
	int shortest = MAXDIST, sdist;

	if( !io.canread ) {
		return;
	}
	io.canread = 0;
	io.cansend = 1;
	M->ip += 2;

	/* Prefer Unvisited nodes first */
	if( !visited( D.x, D.y - 1 ) ) {
		D.dir = north;
		writemem( M, M->ip - 1, north);
		return;
	} else {
		shortest = dist( D.x, D.y - 1 );
		sdist = north;
	}
		
	if( !visited( D.x - 1, D.y ) ) {
		D.dir = west;
		writemem( M, M->ip - 1, west);
		return;
	} else {
		if( dist( D.x - 1, D.y ) < shortest ) {
			shortest = dist( D.x - 1, D.y );
			sdist = west;
		}
	}
	if( !visited( D.x, D.y + 1 ) ) {
		D.dir = south;
		writemem( M, M->ip - 1, south);
		return;
	} else {
		if( dist( D.x, D.y + 1 ) < shortest ) {
			shortest = dist( D.x, D.y + 1 );
			sdist = south;
		}
	}
	if( !visited( D.x + 1, D.y ) ) {
		D.dir = east;
		writemem( M, M->ip - 1, east);
		return;
	} else {
		if( dist( D.x + 1, D.y ) < shortest ) {
			shortest = dist( D.x + 1, D.y );
			sdist = east;
		}
	}
	D.dir = sdist;
	writemem( M, M->ip - 1, sdist );	
}

void
icm_out(Intcode *M)
{
	vlong value;
	if( !io.cansend ) {
		return;
	}
	io.cansend = 0;
	io.canread = 1;

	value = readmem( M, M->ip + 1 );
	RUNNINGP;

	switch( value ) {
		case WALL:
			switch( D.dir ) {
			case north:
				visit( D.x, D.y - 1 );
				fill( D.x, D.y - 1, wall );
				break;
			case south:
				visit( D.x, D.y + 1 );
				fill( D.x, D.y + 1, wall );
				break;
			case west:
				visit( D.x - 1, D.y );
				fill( D.x - 1, D.y, wall );
				break;
			case east:
				visit( D.x + 1, D.y );
				fill( D.x + 1, D.y, wall );
				break;
			default:
				break;
			}
			break;
		case MOVE:
			switch( D.dir ) {
			case north:
				visit( D.x, D.y - 1 );
				fill( D.x, D.y - 1, known );
				if( dist(D.x, D.y - 1) > dist(D.x, D.y) + 1 ) {
					setdist( D.x, D.y - 1, dist(D.x, D.y) + 1 );
				}
				D.y--;
				break;
			case south:
				visit( D.x, D.y + 1 );
				fill( D.x, D.y + 1, known );
				if( dist(D.x, D.y + 1) > dist(D.x, D.y) + 1 ) {
					setdist( D.x, D.y + 1, dist(D.x, D.y) + 1 );
				}
				D.y++;
				break;
			case west:
				visit( D.x - 1, D.y );
				fill( D.x - 1, D.y, known );
				if( dist(D.x - 1, D.y) > dist(D.x, D.y) + 1 ) {
					setdist( D.x - 1, D.y, dist(D.x, D.y) + 1 );
				}
				D.x--;
				break;
			case east:
				visit( D.x + 1, D.y );
				fill( D.x + 1, D.y, known );
				if( dist(D.x + 1, D.y) > dist(D.x, D.y) + 1) {
					setdist( D.x + 1, D.y, dist(D.x, D.y) + 1 );
				}
				D.x++;
				break;
			default:
				break;
			}
			break;
		case AIR:
			switch( D.dir ) {
			case north:
				visit( D.x, D.y - 1 );
				fill( D.x, D.y - 1, goal );
				if( dist(D.x, D.y - 1) > dist(D.x, D.y) + 1 ) {
					setdist( D.x, D.y - 1, dist(D.x, D.y) + 1 );
				}
				D.y--;
				break;
			case south:
				visit( D.x, D.y + 1 );
				fill( D.x, D.y + 1, goal );
				if( dist(D.x, D.y + 1) > dist(D.x, D.y) + 1 ) {
					setdist( D.x, D.y + 1, dist(D.x, D.y) + 1 );
				}
				D.y++;
				break;
			case west:
				visit( D.x - 1, D.y );
				fill( D.x - 1, D.y, goal );
				if( dist(D.x - 1, D.y) > dist(D.x, D.y) + 1 ) {
					setdist( D.x - 1, D.y, dist(D.x, D.y) + 1 );
				}
				D.x--;
				break;
			case east:
				visit( D.x + 1, D.y );
				fill( D.x + 1, D.y, goal );
				if( dist(D.x + 1, D.y) > dist(D.x, D.y) + 1 ) {
					setdist( D.x + 1, D.y, dist(D.x, D.y) + 1 );
				}
				D.x++;
				break;
			default:
				break;
			}
			G.x = D.x;
			G.y = D.y;
			if( !foundo ) {
				foundo = 1;
				reset_dist();
				maxdist = 0;
				setdist( D.x, D.y, 0 );
				visit( D.x, D.y );
			}
			break;
		default:
			print("Bad response from droid.\n");
			M->state = ERRMEM;
			break;
	}

	if( D.x == flr.min_x ) {
		resize_map( D.x - 1, flr.max_x, flr.min_y, flr.max_y );
	}
	if( D.y == flr.min_y ) {
		resize_map( flr.min_x, flr.max_x, D.y - 1, flr.max_y );
	}
	if( D.x == flr.max_x ) {
		resize_map( flr.min_x, D.x + 1, flr.min_y, flr.max_y );
	}
	if( D.y == flr.max_y ) {
		resize_map( flr.min_x, flr.max_x, flr.min_y, D.y + 1 );
	}

	M->ip += 2;
}

void
create_map()
{
	flr.v = nil;
	resize_map(0, 2, 0, 2);
	flr.v[4].val = start;
	flr.v[4].dist = 0;
	flr.v[4].visited = 1;
}

void
reset_dist()
{
	int m, i, q;
	q = (flr.max_x - flr.min_x + 1) * (D.y - flr.min_y) + (D.x - flr.min_x);
	m = (flr.max_x - flr.min_x + 1) * (flr.max_y - flr.min_y + 1);
	
	for( i = 0; i < m; i++ ) {
		if( i != q ) {
			flr.v[i].dist = MAXDIST;
			flr.v[i].visited = 0;
		} else {
			flr.v[i].dist = 0;
		}
	}
}

void
resize_map(int x0, int x1, int y0, int y1)
{
	int newsize, i, j, ysum, psum;
	Loc *newfloor = nil;

	newsize = (x1 - x0 + 1) * (y1 - y0 + 1);
	if( !( newfloor = malloc( newsize * sizeof(Loc) ) ) ) {
		exits("Failed to resize floor.");
	}

	for( i = 0; i < newsize; i++ ) {
		newfloor[i].val = unknown;
		newfloor[i].visited = 0;
		newfloor[i].dist = MAXDIST;
	}

	if( flr.v ) {
		for( i = flr.min_y; i <= flr.max_y; i++ ) {
			ysum = (i - y0) * (x1 - x0 + 1);
			psum = (i - flr.min_y) * (flr.max_x - flr.min_x + 1);
			for( j = flr.min_x; j <= flr.max_x; j++ ) {
				newfloor[ ysum + (j - x0) ].val = flr.v[ psum + (j - flr.min_x) ].val;
				newfloor[ ysum + (j - x0) ].dist = flr.v[ psum + (j - flr.min_x) ].dist;
				newfloor[ ysum + (j - x0) ].visited = flr.v[ psum + (j - flr.min_x) ].visited;
			}
		}
		free(flr.v);
	}
	flr.min_x = x0;
	flr.max_x = x1;
	flr.min_y = y0;
	flr.max_y = y1;
	flr.v = newfloor;
}

void
print_map()
{
	int i, j, ysum;

	for( j = flr.min_y; j <= flr.max_y; j++ ) {
		ysum = (j - flr.min_y) * (flr.max_x - flr.min_x + 1);
		for( i = flr.min_x; i <= flr.max_x; i++ ) {
			if( i == D.x && j == D.y ) {
				print("* ");
			} else {
				print("%c ", flr.v[ ysum + (i - flr.min_x) ].val );
			}
		}
		print("\n");
	}
	print("\n");
}

void
init_droid()
{
	D.dir = north;
	D.x = 1;
	D.y = 1;
}

void
fill(int x, int y, int v)
{
	if( x == 1 && y == 1 ) {
		return;
	}
	flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].val = v;
}

void
visit(int x, int y)
{
	flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].visited = 1;
}

void
setdist(int x, int y, int v)
{
	if( v > maxdist ) {
		print("%d\n", v);
		maxdist = v;
	}
	flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].dist = v;
}

int
val(int x, int y)
{
	return flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].val;
}

int
visited(int x, int y)
{
	return flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].visited;
}

int
dist(int x, int y)
{
	return flr.v[ (y - flr.min_y) * (flr.max_x - flr.min_x + 1) + (x - flr.min_x) ].dist;
}
