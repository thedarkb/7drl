#include <stdlib.h>
#include <stdio.h>

#define BITSET(array,x) array|=1<<(15-(x))
#define BITCLEAR(array,x) array&=~(1<<15-(x))
#define BITGET(array,x) ((array&1<<(15-(x)))?1:0)

#define NEW(x) malloc(sizeof (x) )

#define GOTOXY(x,y) printf("%c[%d;%df",0x1B,y,x)
#define CLEAR(x) printf("%c[%dJ",0x1B,x)
#define COLOUR(x) printf("%c[%dm",0x1B,x+30)

enum {
	ENT_PLAYER,
	ENT_PASSIVE,
	ENT_AGGRESSIVE,
	ENT_SHOPKEEPER,
	ENT_BOSS
};

enum {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_WARP,
	KEY_BOMB,
	KEY_WAIT
};

enum 
{
	RUN_ROOM,
	/*Entity is freed when out of the room.*/
	RUN_PERSIST,
	/*Entity survives being in a different room, but its turn handler isn't called.*/
	RUN_ALWAYS,
	/*Entity's turn handler is called on every turn, even when out of the room.*/
	RUN_DIE
	/*Cease to exist at the earliest convenience.*/
};

struct entity {/*12 bytes in real mode, ideally.*/
	char type;
	char runWhen;
	char sX,sY;
	char x,y;
	int (*turn)(struct entity*);
	void* state;
	struct entity* next;
};

typedef struct entity entity;

entity* entList;
