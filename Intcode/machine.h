#define SIZE	1500
#define POSITION 0
#define IMMEDIATE 1
#define RELATIVE 2
#define ADD	1
#define MULT	2
#define	IN	3
#define OUT	4
#define JT	5
#define JF	6
#define LT	7
#define EQ	8
#define ADJRB	9
#define HALT	99
#define STOPPED	23
#define RUNNING 24
#define ERROP	25
#define ERRMEM	26
#define ERRMODE	27
#define ERRIN	28
#define ERRFILE	-1

#define RUNNINGP if( IM.state != RUNNING ) { return; }

struct Intcode_machine {
	long ip;
	long base;
	long inst;
	long mode;
	long state;
	vlong mem[SIZE];
} IM;

/* Functions in Icode-init.c */
int print_mem(long, long);
long populate(void);
int init(void);
int valid(long);
char *print_state(void);

/* Functions in Icode-op.c */
void step(void);
void icm_add(void);
void icm_mult(void);
void icm_halt(void);
void icm_in(void);
void icm_out(void);
void icm_jt(void);
void icm_jf(void);
void icm_lt(void);
void icm_eq(void);
void icm_adjrb(void);
vlong readmem(long);
void writemem(long, vlong);
