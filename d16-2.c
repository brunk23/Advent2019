#include <u.h>
#include <libc.h>
#include <stdio.h>

/* There are 651 characters in the input file */
#define LENGTH 700
#define REPEAT 10000

/* Yeah, we're going to use globals here */
int *signal;
char read_buff[LENGTH];
int sig_length;
int offset;

int is_alph_num( char );
void set_buffers( void );
void correction_pass( void );
void print_signal( void );

void
print_signal()
{
	int i;

	for( i = 0; i < 8; i++ ) {
		print("%d", signal[ i ] );
	}
	print("\n");
}

/*
 * Because our offset is 5.9 million, and the max is 6.5 million, we always
 * add the digits after it. We can work backwards to complete this in one pass
 */
void
correction_pass()
{
	int sum = 0, i;
	for( i = sig_length - 1; i >= 0; i-- ) {
		sum += signal[ i ];
		sum %= 10;
		signal[ i ] = sum;
	}
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
	int i,j = 0, max;
	max = strlen(read_buff) - 1;
	i = offset % max;

	while( j < sig_length ) {
		signal[ j ] = read_buff[ i ] - '0';
		j++;
		i = (i + 1) % max;
	}
}

void
main(int argc, char *argv[])
{
	FILE *fp = nil;
	int i, j;

	if( !( fp = fopen("d16-input","r") ) ) {
		exits("Can't open file for reading.");
	}

	if( !( fgets(read_buff, LENGTH, fp) )) {
		exits("Did not read a line.");
	}

	offset = 0;
	i = strlen( read_buff ) - 1;

	for( j = 0; j < 7; j++ ) {
		offset *= 10;
		offset += read_buff[ j ] - '0';
	}

	i *= REPEAT;
	i -= offset;

	sig_length = i;
	if(!(signal = malloc ( i * sizeof(int) ) ) )
	{
		exits("Could not get enough memory.");
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
