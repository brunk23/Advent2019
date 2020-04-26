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

	if( IM.ip > IM.highest ) {
		IM.state = STOPPED;
		return;
	}
	
	IM.inst = IM.mem[ IM.ip ] % 100;
	IM.mode = IM.mem[ IM.ip ] / 100;

	print("%d:\t", IM.ip);
	IM.ip++;
	switch( IM.inst ) {
		case HALT:
			print("halt");
			break;
		case ADD:
			one();
			print(" + ");
			one();
			print(" -> ");
			one();
			break;
		case MULT:
			one();
			print(" * ");
			one();
			print(" -> ");
			one();
			break;
		case LT:
			one();
			print(" < ");
			one();
			print(" -> ");
			one();
			break;
		case EQ:
			one();
			print(" == ");
			one();
			print(" -> ");
			one();
			break;
		case JT:
			print("if\t");
			one();
			print("\tgoto\t");
			one();
			break;
		case JF:
			print("ifnot\t");
			one();
			print("\tgoto\t");
			one();
			break;
		case OUT:
			print("output from <- ");
			one();
			break;
		case IN:
			print("input to -> ");
			one();
			break;
		case ADJRB:
			print("base + ");
			one();
			print(" -> base");
			break;
		default:
			print("DATA    $%lld", IM.mem[ IM.ip -1 ] );
			break;
	}
	print("\n");
}

void one() {
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
			print("(%d)", IM.mode % 10);
			break;
	}
	print("%lld", IM.mem[ IM.ip ] );
	IM.ip++;
	IM.mode /= 10;
}
