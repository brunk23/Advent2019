#include <u.h>
#include <libc.h>
#include <stdio.h>

#define SIZE 500
#define ERRMEM -999
#define ERROP -998
#define ADD 1
#define MULT 2
#define HALT 99

struct machine {
	int ip;
	int op;
	int right;
	int left;
	int dest;
	int max;
	int mem[SIZE];
	int omem[SIZE];
} m1202;

void init_machine(void) {
	int i;

	m1202.ip = 0;
	m1202.op = 0;
	m1202.right = 0;
	m1202.left = 0;
	m1202.dest = 0;
	for( i = 0; i < SIZE; i++ ) {
		m1202.mem[i] = m1202.omem[i];
	}
}

int valid(int i) {
	if( i >= 0 && i < SIZE ) {
		return 1;
	}
	return 0;
}

/*
 * Read value at ip and increment ip.
 * Returns value on success, ERRMEM on failure
 */
int read_ip(void) {
	if( valid( m1202.ip ) ) {
		m1202.ip++;
		return m1202.mem[ m1202.ip - 1 ];
	}
	return ERRMEM;
}

/*
 * Write value to ip and increment it.
 * Returns 0 on success, ERRMEM on failure
 */
int write_ip(int value) {
	if( valid( m1202.dest ) ) {
		m1202.mem[ m1202.dest ] = value;
		return 0;
	}
	return ERRMEM;
}

int val( int n ) {
	if( valid( n ) ) {
		return m1202.mem[n];
	}
	return ERRMEM;
}

int step(void) {
	m1202.op = read_ip();

	/* Halt on end and memory error */
	if( m1202.op == HALT || m1202.op == ERRMEM ) {
		return m1202.op;
	}

	m1202.right = val(read_ip());
	m1202.left = val(read_ip());
	m1202.dest = read_ip();

	if( m1202.right == ERRMEM || m1202.left == ERRMEM ) {
		return ERRMEM;
	}

	if( m1202.op == ADD ) {
		if( write_ip( m1202.right + m1202.left ) != 0 ) {
			return ERRMEM;
		}
		return 1;
	}

	if( m1202.op == MULT ) {
		if( write_ip( m1202.right * m1202.left ) != 0 ) {
			return ERRMEM;
		}
		return 1;
	}

	return ERROP;
}

int populate(void) {
	int n;

	for( n = 0; n < SIZE; n++ ) {
		m1202.mem[n] = 0;
	}

	while( EOF != ( scanf("%d,", &m1202.omem[ m1202.max++ ] )) ) {
		/* empty */
	}
	return 0;
}

void printmem(void) {
	int n = 0;
	for( n = 0; n < m1202.max; n++ ) {
		print("%d\t", m1202.mem[n]);
		if( n%10 == 9 ) {
			print("\n");
		}
	}
}

int main(int argc, char *argv[]) {
	int val = 0, x, y;

	m1202.max = 0;
	populate();

	for( x = 0; x < 100; x++ ) {
		for( y = 0; y < 100; y++ ) {

			init_machine();
			val = 0;

			m1202.mem[1] = x;
			m1202.mem[2] = y;

			while (val != HALT && val != ERRMEM &&
				 val != ERROP ) {
				val = step();
			}

			print("%d:%d:%d\n",x,y,m1202.mem[0]);

			if( m1202.mem[0] == 19690720 ) {
				print("%d\n\n", 100*x+y);
				return 0;
			}
		}
	}

	printmem();
	print("size = %d\nip = %d\nmem[0] = %d\n",
		m1202.max, m1202.ip, m1202.mem[0]);
	return 0;
}
