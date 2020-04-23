#define HULLSIZE	10000
#define NEWSPOT		-1
#define UP		0
#define LEFT		3
#define RIGHT		1
#define DOWN		2
#define PAINT		1
#define DIRECT		2
#define BLACK		0
#define WHITE		1

typedef struct HullSpot HullSpot;
typedef struct Robot Robot;

struct HullSpot {
	int x;
	int y;
	int val;
	int paint;
};

HullSpot hull[HULLSIZE];

struct Rotbot {
	int direction;
	int state;
	int x;
	int y;
	int minx;
	int maxx;
	int miny;
	int maxy;
	int val;
	HullSpot *loc;
} robot;

vlong robot_read(void);
void robot_move(void);
