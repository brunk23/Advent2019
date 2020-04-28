#include <u.h>
#include <libc.h>
#include <stdio.h>

#define MAXLEN 120
#define MAXLEFT 10
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
	int amount_made;
	Token needs[ MAXLEFT ];
};

enum
{
	leftamount, leftword, rightamount, rightword, unknown
};

Elem *parse_line( char *n );
void print_element( Elem * );

void
print_element( Elem *e ) {
	int i;
	print("To make %d of %s, we need ", e->amount_made, e->name);
	for( i = 0; i < MAXLEFT; i++ ) {
		if( e->needs[i].amount > 0 ) {
			print("%d of %s, ", e->needs[i].amount, e->needs[i].name);
		}
	}
	print("\n");
}

Elem *parse_line(char *s)
{
	long i = 0;
	int state = leftamount;
	Elem *e = nil;

	print("%d\n", sizeof(Elem));

	if( !( e = malloc( sizeof(Elem) ) ) ) {
		exits("Can't get memory.");
	}

	for( i = 0; i < MAXLEFT; i++ ) {
		e->needs[i].amount = 0;
	}

	i = 0;
	s = strtok(s, ", \r\t\n");
	print("[%s] ", s);
	sscanf( s, "%d", &e->needs[i].amount);
	state = leftword;

	while( s = strtok(0 , ", \r\t\n") ) {
		if( 0 == strcmp( s, "=>" ) ) {
			state = rightamount;
			continue;
		}
		switch( state ) {
		case leftamount:
			print("[%s] ", s);
			sscanf( s, "%d", &e->needs[i].amount);
			state = leftword;
			break;
		case leftword:
			print("[%s] ", s);
			strncpy( e->needs[i].name, s, NAMESIZE);
			state = leftamount;
			i++;
			break;
		case rightamount:
			print("[%s] ", s);
			sscanf( s, "%d", &e->amount_made);
			state = rightword;
			break;
		case rightword:
			print("[%s] ", s);
			strncpy( e->name, s, NAMESIZE);
			state = unknown;
			break;
		default:
			print("Error, got tokens after right word.\n");
			break;
		}
	}
	print("\n");


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

	for( i = 0; i < NUMBER; i++ ) {
		all[i] = nil;
	}

	if( !( fp = fopen("d14-input", "r") ) ) {
		exits("Can't open file");
	}
	i = 0;
	while( (s = fgets( n, MAXLEN, fp )) ) {
		all[i] = parse_line( s );
		i++;
	}

	for( i = 0; i < NUMBER; i++ ) {
		if( all[i] ) {
			print_element( all[i] );
		}
	}

	for( i = 0; i < NUMBER; i++ ) {
		if( all[i] ) {
			free( all[ i ] );
		}
	}

	fclose(fp);
	exits(0);
}
