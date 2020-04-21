#define WSIZE	800000
#define EMPTY	-1
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
	vlong ip;
	vlong base;
	vlong inst;
	vlong mode;
	vlong state;
	vlong mem[WSIZE][2];
} IM;

/* Functions in Icode-init.c */
int print_mem(void);
vlong populate(void);
int init(void);
vlong *valid(vlong);
void addpair(vlong, vlong);
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
vlong readmem(vlong);
void writemem(vlong, vlong);
