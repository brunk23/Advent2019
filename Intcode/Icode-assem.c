#include <u.h>
#include <libc.h>
#include <stdio.h>

#define MAXLEN 120

enum
{
	ADD=1, MULT, IN, OUT, JT, JF, LT, EQ, ADJRB,
	HALT=99, POSITION, IMMEDIATE, RELATIVE, VAR
};

typedef struct TOKEN TOKEN;
typedef struct LINE LINE;
typedef struct RESERVED RESERVED;

struct RESERVED
{
	char *name;
	int value;
};

struct TOKEN
{
	int type;
	int line;
	int location;
	int resolved;
	char *value;
	TOKEN *next;
};

struct LINE
{
	int line_no;
	char *value;
	LINE *next;
};


RESERVED reserved_words[] = {
	{"add", ADD},
	{"mult", MULT},
	{"in", IN},
	{"out", OUT},
	{"jt", JT},
	{"jf", JF},
	{"lt", LT},
	{"eql", EQ},
	{"adjb", ADJRB},
	{"halt", HALT},
	{"var", VAR},
	{nil, 0}
};

void process_input( char *filename );
LINE *create_line( LINE *prev, int numb, char *str );
TOKEN *create_tokens( TOKEN *prev, int numb, char *str );
void free_lines( void );
void free_tokens( void );
void print_lines( void );
void print_tokens (void );
int is_reserved( char * );

/*
 * Global value trees
 */
TOKEN *tokens;
LINE *lines;

void
main(int argc, char *argv[])
{
	tokens = nil;
	lines = nil;

	if( argc >= 2 ) {
		process_input( argv[1] );
	} else {
		process_input( 0 );
	}
	print_tokens();

	free_tokens();
	free_lines();
	exits(0);
}

int
is_reserved( char *s ) {
	int i = 0;

	while( reserved_words[i].name ) {
		if( !( strcmp( reserved_words[i].name, s ) ) ) {
			return reserved_words[i].value;
		}
		i++;
	}
	return 0;
}

void
process_input( char *filename )
{
	FILE *fp_in = nil;
	LINE *curr_line = lines;
	TOKEN *curr_token = tokens;
	int linenumber = 0;
	char input[MAXLEN];

	if( filename ) {
		fp_in = fopen( filename, "r");
		if( !(fp_in) ) {
			fprintf( stderr, "Can't open input file %s\n", filename );
			exits("File Error");
		}
	} else {
		fp_in = stdin;
	}

	while( fgets( input, MAXLEN, fp_in ) ) {
		/*
		 * We should only increase linenumber if we find a newline.
		 * and we should concatonate long strings, not that we
		 * should need them for this program.  For now, we will
		 * assume source files play nice.
		 */
		linenumber++;
		curr_line = create_line( curr_line, linenumber, input );
		curr_token = create_tokens( curr_token, linenumber, input );
	}

	if( fp_in && fp_in != stdin ) {
		fclose( fp_in );
	}
}

LINE *
create_line( LINE *prev, int numb, char *str )
{
	LINE *curr = nil;
	int i;

	if( !( curr = malloc( sizeof( LINE ) ) ) ) {
		exits("No space for new line.");
	}

	if( !prev ) {
		lines = curr;
	} else {
		prev->next = curr;
	}

	curr->next = nil;
	curr->line_no = numb;

	/*
	 * Strip the trailing newline.
	 */
	for( i = 0; i < MAXLEN; i++ ) {
		if( str[i] == '\n' ) {
			str[i] = '\0';
			break;
		}
	}

	if( !( curr->value = malloc( sizeof(char) * (strlen(str) + 1) ) ) ) {
		exits("Couldn't allocate room to store string.");
	}
	strncpy( curr->value, str, strlen(str) + 1 );

	return curr;
}

void
free_lines()
{
	LINE *curr = lines, *next;

	while( curr ) {
		next = curr->next;
		free( curr->value );
		free( curr );
		curr = next;
	}
}

void
print_lines()
{
	LINE *curr = lines;

	while( curr ) {
		print("%d:\t%s\n", curr->line_no, curr->value );
		curr = curr->next;
	}
}

TOKEN *
create_tokens( TOKEN *prev, int numb, char *str )
{
	TOKEN *curr = nil, *p;
	char *s = str;
	int i;

	p = prev;

	/* Get rid of comments */
	for( i = 0; i < strlen(str); i++ ) {
		if( str[i] == ';' ) {
			str[i] = '\0';
			break;
		}
	}

	s = strtok(s, " \t\n\r");

	while( s ) {
		if( !( curr = malloc( sizeof( TOKEN ) ) ) ) {
			exits("Couldn't create new token.");
		}
		if( !( curr->value = malloc( sizeof( char ) * (strlen( s ) + 1) ) ) ) {
			exits("Couldn't create token string.");
		}

		strncpy( curr->value, s, strlen( s ) + 1 );
		curr->type = is_reserved( s );
		curr->next = nil;
		curr->location = -1;
		curr->resolved = 0;
		curr->line = numb;

		print("Made token for %s\n", s);
		if( !tokens ) {
			tokens = curr;
			p = curr;
		} else {
			p->next = curr;
			p = curr;
		}
		s = strtok( 0, " \t\n\r");
	}

	return curr;
}

void
free_tokens()
{
	TOKEN *curr = tokens, *next;

	while( curr ) {
		next = curr->next;
		free( curr->value );
		free( curr );
		curr = next;
	}
}

void
print_tokens()
{
	TOKEN *curr = tokens;

	while( curr ) {
		print("T: %s\t%d\t%d\t%d\n",curr->value,curr->type, curr->location, curr->line);
		curr = curr->next;
	}
}
