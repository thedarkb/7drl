#include "main.h"
#include "globals.h"

extern int sp_readchar();
extern int external_walls();
extern int collisionGet();

struct playerstate {
	char drawn;
	char lastX;
	char lastY;
};

static int
turn_player(this)
	entity* this;
{
	auto struct playerstate* state;
	static int logged=1;
	static int regen=1;
	char input;
	int tempX=this->x;
	int tempY=this->y;
	int hideX=this->x;
	int hideY=this->y;

	state=this->state;

	if(!state->drawn)
	{
		state->drawn=1;
	}
	
	while(!logged)
	{
		switch(sp_readchar())
		{
			case 'h':
			case 'H':
				logged=1;
				tempX--;
				break;
			case 'l':
			case 'L':
				logged=1;
				tempX++;
				break;
			case 'j':
			case 'J':
				logged=1;
				tempY++;
				break;
			case 'k':
			case 'K':
				logged=1;
				tempY--;
				break;
		}
	}
	if(logged) 
	{
		if(!collisionGet(tempX,tempY))
		{
			this->x=tempX;
			this->y=tempY;
		}
		else logged=0;
	}
	if(logged)
	{
		if(this->x>79) 
		{
			this->x=2;
			sX++;
			regen=1;
		}
		if(this->x<2) 
		{
			this->x=78;
			sX--;
			regen=1;

		}
		if(this->y>22) 
		{
			this->y=2;
			sY++;
			regen=1;

		}
		if(this->y<2) 
		{
			this->y=22;
			sY--;
			regen=1;
		}
		if(regen)
		{
			CLEAR(0);
			external_walls();
			regen=0;
		}
		

		GOTOXY(hideX,hideY);
		putchar(' ');
		GOTOXY(this->x,this->y);
		putchar('@');
		GOTOXY(this->x,this->y-1);
		putchar('\n');
		logged=0;
	}
}

entity*
construct_player()
{
	auto entity* player;
	auto struct playerstate* state;

	player=NEW(entity);	

	player->type=ENT_PLAYER;
	player->runWhen=RUN_ALWAYS;
	player->sX=7;
	player->sY=7;
	player->x=2;
	player->y=2;
	player->turn=turn_player;
	player->state=NEW(struct playerstate);
	state=player->state;
	player->next=NULL;

	state->drawn=0;

	return player;
}
