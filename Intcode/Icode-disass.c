#include <u.h>
#include <libc.h>
#include "machine.h"

void dis_step(void);
void one(void);
void two(void);
void three(void);

char *opwords[10] =
{
"halt    ", "add     ", "mult    ", "in      ", "out     ",
"jt      ", "jf      ", "lessthan", "equalto ", "adjrelb "
};

void
main( int argc, char *argv[])
{
	init();
	populate();

	IM.state = RUNNING;

	while( IM.state == RUNNING ) {
		dis_step();
	}

	exits(0);
}

void 
dis_step()
{
	int n = 0;
	char *s = nil;

	if( IM.mem[ IM.ip ][0] == -1 ) {
		IM.state = STOPPED;
		return;
	}
	
	IM.inst = IM.mem[ IM.ip ][1] % 100;
	IM.mode = IM.mem[ IM.ip ][1] / 100;

	switch( IM.inst ) {
		case HALT:
			n = 0;
			s = opwords[0];
			break;
		case ADD:
		case MULT:
		case LT:
		case EQ:
			n = 3;
			s = opwords[ IM.inst ];
			break;
		case JT:
		case JF:
			n = 2;
			s= opwords[ IM.inst ];
			break;
		case OUT:
		case IN:
		case ADJRB:
			n = 1;
			s = opwords[ IM.inst ];
			break;
		default:
			print("%d:\tDATA    ", IM.ip++);
			s = 0;
			IM.mode = 1;
			one();
			break;
	}
	if( s ) {
		print("%d:\t%s", IM.ip++, s);
		for( ; n; n-- ) {
			one();
		}
	}
	print("\n");
}

void one() {
	print("\t");
	switch( IM.mode % 10 ) {
		case 0:
			print("@");
			break;
		case 1:
			print("$");
			break;
		case 2:
			print("#");
			break;
		default:
			print("??-");
			break;
	}
	print("%lld", IM.mem[ IM.ip ][1] );
	IM.ip++;
	IM.mode /= 10;
}
