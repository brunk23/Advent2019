#include <u.h>
#include <libc.h>
#include <stdio.h>

/* There are 651 characters in the input file */
#define LENGTH 700

/* Yeah, we're going to use globals here */
int pattern[4];
int orig_signal[LENGTH];
int signals[2][LENGTH];
char read_buff[LENGTH];
int sig_length;

int is_alph_num( char );
void set_pattern( void );
void set_buffers( void );

int
is_alph_num( char a )
{
	return ( (a >= '0') && (a <= '9') );
}

/*
 * This will process the readbuff and fill orig_signal and signals[0] with the
 * pattern read from the file. signals[1] will be zeroed out.
 * It also will set sig_length for speedier processing of the buffers
 */
void
set_buffers()
{
	int i;

	set_pattern();

	sig_length = 0;

	for( i = 0; i < LENGTH; i++ ) {
		if( is_alph_num( read_buff[i] ) ) {
			sig_length++;
			orig_signal[ i ] = read_buff[ i ] - '0';
			signals[ 0 ][ i ] = orig_signal[ i ];
		} else {
			orig_signal[ i ] = 0;
			signals[ 0 ][ i ] = 0;
		}
		signals[ 1 ][ i ] = 0;
	}
}

void
set_pattern()
{
	pattern[0] = 0;
	pattern[1] = 1;
	pattern[2] = 0;
	pattern[3] = -1;
}

void
main(int argc, char *argv[])
{
	FILE *fp = nil;

	if( !( fp = fopen("d16-input","r") ) ) {
		exits("Can't open file for reading.");
	}

	if( !( fgets(read_buff, LENGTH, fp) )) {
		exits("Did not read a line.");
	}

	/* Must be done after reading string */
	set_buffers();

	fclose(fp);
	exits(0);
}
