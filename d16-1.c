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
int curr_sig;

int is_alph_num( char );
void set_pattern( void );
void set_buffers( void );
void correction_pass( void );
void print_signal( void );

void
print_signal()
{
	int i;

	for( i = 0; i < 8; i++ ) {
		print("%d", signals[ curr_sig ][ i ] );
	}
	print("\n");
}

void
correction_pass()
{
	int finding = 1, corr_fact = 0, corr_mult = 1, sum = 0, i;

	while( finding <= sig_length ) {
		for( i = 0; i < sig_length; i++ ) {
			if( finding == corr_mult ) {
				corr_fact++;
				corr_fact %= 4;
				corr_mult = 0;
			}
			corr_mult++;
			sum += signals[ curr_sig ][ i ] * pattern[ corr_fact ];
		}

		if( sum < 0 ) {
			sum *= -1;
		}
		sum %= 10;
		signals[ (curr_sig + 1) % 2 ][ finding - 1 ] = sum;

		corr_fact = 0;
		corr_mult = 1;
		sum = 0;
		finding++;
	}
	curr_sig += 1;
	curr_sig %= 2;
}


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
	curr_sig = 0;

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
	int i;

	if( !( fp = fopen("d16-input","r") ) ) {
		exits("Can't open file for reading.");
	}

	if( !( fgets(read_buff, LENGTH, fp) )) {
		exits("Did not read a line.");
	}

	/* Must be done after reading string */
	set_buffers();

	for( i = 0; i < 100; i++ ) {
		correction_pass();
		print_signal();
	}

	fclose(fp);
	exits(0);
}
