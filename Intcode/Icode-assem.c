#include <u.h>
#include <libc.h>
#include <stdio.h>

#define MAXLEN 120

enum
{
	ADD=1, MULT, IN, OUT, JT, JF, EQ, ADJRB,
	HALT=99, POSITION, IMMEDIATE, RELATIVE
};

typedef struct TOKEN TOKEN;
typedef struct LINE LINE;

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


void process_input( char *filename );
LINE *create_line( LINE *prev, int numb, char *str );
void free_lines( void );
void print_lines( void );

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
	print_lines();

	free_lines();
	exits(0);
}

void
process_input( char *filename )
{
	FILE *fp_in = nil;
	LINE *curr = lines;
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
		curr = create_line( curr, linenumber, input );
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
