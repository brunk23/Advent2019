#include <u.h>
#include <libc.h>
#include <stdio.h>

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

int gravity(int, int);
void display_planet_info(Planet *);
void update_velocities(Planet *);
void update_positions(Planet *);
int energy(Planet *);
int total_energy(Planet *);

int
energy(Planet *p)
{
	int i;

	i = abs( p->pos.x ) + abs( p->pos.y ) + abs( p->pos.z );
	i *= ( abs( p->vel.x ) + abs( p->vel.y ) + abs( p->vel.z ) );
	return i;
}

int
total_energy(Planet *p)
{
	int i, total = 0;
	for( i = 0; i < 4; i++ ) {
		total += energy( &p[i] );
	}
	return total;
}

void
display_planet_info(Planet *p)
{
	int i;
	for( i = 0; i < 4; i++ ) {
		print("Pos=<x=%d, y=%d, z=%d> ", p[i].pos.x, p[i].pos.y, p[i].pos.z);
		print("Vel=<x=%d, y=%d, z=%d>\n", p[i].vel.x, p[i].vel.y, p[i].vel.z);
	}
	print("Total: %d\n", total_energy(p) );
	print("\n");
}

int
gravity(int a, int b)
{
	if( a > b ) {
		return -1;
	}
	if( b > a ) {
		return 1;
	}
	return 0;
}

void
update_velocities(Planet *p)
{
	int i, j;
	for( i = 0; i < 4; i++ ) {
		for( j = 0; j < 4; j++ ) {
			p[i].vel.x += gravity( p[i].pos.x, p[j].pos.x );
			p[i].vel.y += gravity( p[i].pos.y, p[j].pos.y );
			p[i].vel.z += gravity( p[i].pos.z, p[j].pos.z );
		}
	}
}

void
update_positions(Planet *p)
{
	int i;
	for( i = 0; i < 4; i++ ) {
		p[i].pos.x += p[i].vel.x;
		p[i].pos.y += p[i].vel.y;
		p[i].pos.z += p[i].vel.z;
	}
}

void
main(int argc, char *argv[])
{
	FILE *fp;
	Planet planets[4];
	int x, y, z, i = 0;

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

	for( i = 0; i <= 1000; i++ ) {
		display_planet_info(planets);
		update_velocities(planets);
		update_positions(planets);
	}

	fclose(fp);
	exits(0);
}
