#include <u.h>
#include <libc.h>
#include <stdio.h>

/* Actual file has 1713 lines and each one is 8 chars and names are 3 chars */
#define MAXORBITS 2000
#define ORBITLINE 8
#define NSIZE 4

typedef struct Orbit Orbit;

struct
Orbit
{
	char name[NSIZE];
	char body[NSIZE];
	Orbit *around;

};

void path_orbits(Orbit *);
int count_orbits(Orbit *);
void find_santa(Orbit *);
int canreach(Orbit *, Orbit *);

void
find_santa(Orbit *o)
{
	Orbit *you = nil, *santa = nil;
	int i, youstep = 0, santastep = 0;

	for( i = 0; i < MAXORBITS; i++ ) {
		if( you && santa ) {
			break;
		}
		if( 0 == strcmp( o[i].body, "YOU" ) ) {
			you = &o[i];
			continue;
		}
		if( 0 == strcmp( o[i].body, "SAN" ) ) {
			santa = &o[i];
		}
	}

	while( you ) {
		you = you->around;
		if( santastep = canreach( you, santa ) ) {
			print("\n%d switches\n", youstep + santastep );
			break;
		}
		youstep++;
	}
}

int
canreach(Orbit *goal, Orbit *path)
{
	int santastep = 0;

	while( path ) {
		santastep++;
		if( 0 == strcmp( goal->name, path->name ) ) {
			return santastep;
		}
		path = path->around;
	}
	return 0;
}

int
count_orbits(Orbit *o)
{
	int i, total = 0;
	Orbit *curr;

	for( i = 0; i < MAXORBITS; i++ ) {
		if( o[i].name[0] == 0 ) {
			break;
		}
		curr = o[i].around;
		total++;
		while( curr ) {
			total++;
			curr = curr->around;
		}
	}
	return total;
}

void
patch_orbits(Orbit *o)
{
	int i, j;

	for( i = 0; i < MAXORBITS; i++ ) {
		if( o[i].name[0] == 0 ) {
			break;
		}
		for( j = 0; j < MAXORBITS; j++ ) {
			if( o[j].name[0] == 0 ) {
				print("%s does not orbit anything\n", o[i].name);
				break;
			}
			if( 0 == strcmp( o[i].name, o[j].body ) ) {
				o[i].around = &o[j];
				break;
			}
		}
	}
}

void
main(int argc, char *argv[])
{
	FILE *fp = nil;
	Orbit *orbits = nil;
	char line[ORBITLINE];	/* Input is well behaved */
	int i = 0, j = 0;

	if( !( fp = fopen("d6-input", "r") ) ) {
		exits("Failed to open file.");
	}

	if( !( orbits = malloc( MAXORBITS * sizeof(Orbit) ) ) ) {
		exits("Failed to get orbit room.");
	}

	for( i = 0; i < MAXORBITS; i++ ) {
		orbits[i].around = nil;
		for( j = 0; j < NSIZE; j++ ) {
			orbits[i].name[j] = 0;
			orbits[i].body[j] = 0;
		}
	}

	/*
	 * This parsing loop is extremely fragile. But should work for this.
	 */
	i = 0;
	while( EOF != ( fscanf(fp, "%s", line) ) ) {
		orbits[i].name[0] = line[0];
		orbits[i].name[1] = line[1];
		orbits[i].name[2] = line[2];
		orbits[i].body[0] = line[4];
		orbits[i].body[1] = line[5];
		orbits[i].body[2] = line[6];
		i++;
	}

	patch_orbits(orbits);

	j = count_orbits(orbits);
	find_santa(orbits);

	print("%d total orbits\n", j);

	free(orbits);
	fclose(fp);
	exits(0);
}
