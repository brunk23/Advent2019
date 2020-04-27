#include <u.h>
#include <libc.h>
#include <stdio.h>

#define STRMAX 4000

enum
{
	up = 'U', down = 'D', left = 'L', right = 'R', done = 'Q'
};

typedef struct Spot Spot;
typedef struct Inst Inst;

struct Inst {
	int direction;
	int value;
	int x;
	int y;
	int wire;
} itoken;

struct Spot {
	int x;
	int y;
	int wire[2];
	Spot *prev;
	Spot *next;
};

Spot *head;
int Spots_used;

Spot *empty_spot();
int fill_itoken(char *, int, int);
int isdir( char );
int isnum( char );
void play_itoken(void);
void addwire(int, int, int);
void addbefore(Spot *, Spot *);
void addafter(Spot *, Spot *);
void delete_Spots( void );
Spot *empty_spot( void );

void delete_Spots()
{
	Spot *curr = head, *tmp;

	while( curr ) {
		print("Spot count: %d\n", Spots_used);
		Spots_used--;
		tmp = curr->next;
		free( curr );
		curr = tmp;
	}
}

void
play_itoken()
{
	int xdir = 0, ydir = 0, i = 0;

	print("%c%d\n", itoken.direction, itoken.value);

	switch( itoken.direction ) {
	case left:
		xdir = -1;
		break;
	case right:
		xdir = 1;
		break;
	case up:
		ydir = -1;
		break;
	case down:
		ydir = 1;
		break;
	defaut:
		break;
	}

	for( i = 0; i <= itoken.value; i++ ) {
		addwire( itoken.x, itoken.y, itoken.wire );
		itoken.x += xdir;
		itoken.y += ydir;
	}
}

void
addwire( int x, int y, int wire )
{
	Spot *curr = head, *tmp = nil;

	tmp = empty_spot();
	tmp->x = x;
	tmp->y = y;
	tmp->wire[wire] = 1;
	Spots_used++;

	/*
	 * curr should always be valid here
	 */
	while( curr ) {
		if( y == curr->y ) {
			if( x == curr->x ) {
				/*
				 * Found spot, mark wire there, free the tmp we don't need
				 */
				curr->wire[wire] = 1;
				Spots_used--;
				free(tmp);
				return;
			}
			if( x < curr->x ) {
				addbefore(curr, tmp);
				return;
			}
			if( curr->next && curr->next->y == y ) {
				curr = curr->next;
				continue;
			}
			addafter(curr, tmp);
			return;
		}
		if( y < curr->y ) {
			addbefore(curr, tmp);
			return;
		}
		if( !curr->next ) {
			addafter(curr, tmp);
			return;
		}
		curr = curr->next;
	}
}

void
addbefore(Spot *curr, Spot *tmp)
{
	tmp->next = curr;
	tmp->prev = curr->prev;
	if( curr->prev ) {
		curr->prev->next = tmp;
	}
	curr->prev = tmp;
	if( curr == head ) {
		head = tmp;
	}
}

void
addafter(Spot *curr, Spot *tmp)
{
	tmp->next = curr->next;
	tmp->prev = curr;
	if( curr->next ) {
		curr->next->prev = tmp;
	}
	curr->next = tmp;
}

int
isnum( char a )
{
	if( a >= '0' && a <= '9' ) {
		return 1;
	}
	return 0;
}

int
isdir( char a )
{
	if( a == up || a == down || a == right || a == left ) {
		return 1;
	}
	return 0;
}

int
fill_itoken(char *s, int start, int max)
{
	int val = 0, dir = done, i = start;

	while( i < max ) {
		if( isdir( s[i] ) ) {
			if( dir == done ) {
				dir = s[i];
			} else {
				break;
			}
		}
		if( isnum( s[i] ) ) {
			val *= 10;
			val += s[i] - '0';
		}
		i++;
	}

	itoken.direction = dir;
	itoken.value = val;

	return i;
}

Spot *
empty_spot()
{
	Spot *s = nil;
	if( !( s = malloc( sizeof( Spot ) ) ) ) {
		exits("Ran out of space making spots.");
	}
	s->prev = nil;
	s->next = nil;
	s->x = 0;
	s->y = 0;
	s->wire[0] = 0;
	s->wire[1] = 0;

	return s;
}

void
main(int argc, char *argv[])
{
	char s[STRMAX];
	FILE *fp;
	int length, currbyte;

	head = empty_spot();
	head->wire[0] = 1;
	head->wire[1] = 1;
	Spots_used = 1;

	itoken.wire = 0;

	if( !( fp = fopen("d3-input","r") ) ) {
		exits("Could not open 'd3-input'");
	}

	while( EOF != fscanf(fp, "%s", s) ) {
		length = strlen(s);
		currbyte = 0;
		itoken.x = 0;
		itoken.y = 0;
		while( currbyte < length ) {
			currbyte = fill_itoken(s, currbyte, length);
			play_itoken();
		}
		itoken.wire++;
	}

	delete_Spots();
	fclose(fp);
}
