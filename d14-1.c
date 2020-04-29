#include <u.h>
#include <libc.h>
#include <stdio.h>

#define MAXLEN 120
#define MAXLEFT 20
#define NAMESIZE 6
#define NUMBER 100

typedef struct Elem Elem;
typedef struct Token Token;

struct Token
{
	char name[ NAMESIZE ];
	int amount;
	Elem *recipe;
};

/*
 * If head of a create list, amount says how much you get
 * If below needs, amount is how much it costs
 */
struct Elem
{
	char name[ NAMESIZE ];
	int increment;
	vlong amount_needed;
	vlong amount_made;
	Token needs[ MAXLEFT ];
};

enum
{
	leftamount, leftword, rightamount, rightword, unknown
};

Elem *parse_line( char *n );
void print_element( Elem * );
void optimize( Elem *[] );
void run_list( Elem *[] );

Elem *fuel;

void
run_list( Elem *a[] )
{
	int change = 1, i, j;
	vlong need, tmp;

	while( change ) {
		change = 0;
		for( i = 0; i < NUMBER; i++ ) {
			if( !a[i] ) {
				break;
			}
			if( a[i]->amount_needed > a[i]->amount_made ) {
				change = 1;
				tmp = a[i]->amount_needed - a[i]->amount_made;
				need = tmp / (vlong) a[i]->increment;
				if( tmp % (vlong) a[i]->increment ) {
					need++;
				}
				a[i]->amount_made += (need * (vlong) a[i]->increment);
				for( tmp = 0; tmp < MAXLEFT; tmp++ ) {
					if( !a[i]->needs[tmp].amount ) {
						break;
					}
					a[i]->needs[tmp].recipe->amount_needed += (need * (vlong) a[i]->needs[tmp].amount);
				}
			}
		}
	}
}

void
optimize( Elem *a[] )
{
	int i = 0, j = 0, n = 0;
	char *s;
	Elem *where;

	for( i = 0; i < NUMBER; i++ ) {
		if( !a[i] ) {
			break;
		}
		s = a[i]->name;
		where = a[i];
		if( !strcmp( s, "FUEL" ) ) {
			a[i]->amount_needed = 1;
			fuel = a[i];
		}
		for( j = 0; j < NUMBER; j++ ) {
			if( !a[j] ) {
				break;
			}
			for( n = 0; n < MAXLEFT; n++ ) {
				if( 0 == a[j]->needs[n].amount ) {
					break;
				}
				if( !strcmp( s, a[j]->needs[n].name ) ) {
					a[j]->needs[n].recipe = where;
				}
			}
		}
	}
}


void
print_element( Elem *e ) {
	int i;
	print("To make %d of %s, we need ", e->increment, e->name);
	for( i = 0; i < MAXLEFT; i++ ) {
		if( e->needs[i].amount > 0 ) {
			print("%d of %s, ", e->needs[i].amount, e->needs[i].name);
		}
	}
	print("\n");
	print("Needed: %lld and Made %lld\n", e->amount_needed, e->amount_made);
}

Elem *parse_line(char *s)
{
	long i = 0;
	int state = leftamount;
	Elem *e = nil;

	if( !( e = malloc( sizeof(Elem) ) ) ) {
		exits("Can't get memory.");
	}

	for( i = 0; i < MAXLEFT; i++ ) {
		e->needs[i].amount = 0;
		e->amount_made = 0;
		e->amount_needed = 0;
	}

	i = 0;
	s = strtok(s, ", \r\t\n");
	sscanf( s, "%d", &e->needs[i].amount);
	state = leftword;

	while( s = strtok(0 , ", \r\t\n") ) {
		if( 0 == strcmp( s, "=>" ) ) {
			state = rightamount;
			continue;
		}
		switch( state ) {
		case leftamount:
			sscanf( s, "%d", &e->needs[i].amount);
			state = leftword;
			break;
		case leftword:
			strncpy( e->needs[i].name, s, NAMESIZE);
			state = leftamount;
			i++;
			break;
		case rightamount:
			sscanf( s, "%d", &e->increment);
			state = rightword;
			break;
		case rightword:
			strncpy( e->name, s, NAMESIZE);
			state = unknown;
			break;
		default:
			print("Error, got tokens after right word.\n");
			break;
		}
	}

	return e;
}

void
main(int argc, char *argv[])
{
	FILE *fp;
	char *s;
	char n[MAXLEN];
	Elem *all[NUMBER];
	int i;
	vlong max;

	max = 1000 * 1000;
	max = max * max;


	for( i = 0; i < NUMBER; i++ ) {
		all[i] = nil;
	}

	if( !( fp = fopen("d14-input", "r") ) ) {
		exits("Can't open file");
	}

	if( !( all[0] = malloc( sizeof(Elem) ) ) ) {
		exits("Can't get element.");
	}

	all[0]->increment = 1;
	all[0]->amount_made = 0;
	all[0]->amount_needed = 0;
	strncpy( all[0]->name, "ORE", NAMESIZE);
	for( i = 0; i < MAXLEFT; i++ ) {
		all[0]->needs[i].amount = 0;
		all[0]->needs[i].name[0] = 0;
	}

	i = 1;
	while( (s = fgets( n, MAXLEN, fp )) ) {
		all[i] = parse_line( s );
		i++;
	}

	optimize( all );

	while( all[0]->amount_made < max ) {
		run_list( all );
		fuel->amount_needed++;
	}

	for( i = 0; i < NUMBER; i++ ) {
		if( all[i] ) {
			print_element( all[i] );
		}
	}

	print("Fuel: %lld\n",fuel->amount_needed - 1);

	for( i = 0; i < NUMBER; i++ ) {
		if( all[i] ) {
			free( all[ i ] );
		}
	}

	fclose(fp);
	exits(0);
}
