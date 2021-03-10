#include "main.h"
#include "entities.h"
#ifdef __unix__
#include <termios.h>
#include <unistd.h>
#endif

unsigned int rngState=6969;
int sX=7;
int sY=7;

/*World data occupying 128 bytes.*/
unsigned int worldArray[16];/*The dungeon as 16 bit fields.*/
unsigned int strengthArray[16];/*The rooms requiring power bracelet to traverse.*/
unsigned int teleportArray[16];/*The rooms requiring teleportation to traverse.*/
unsigned int bombArray[16];/*The rooms requiring bombs to traverse.*/

unsigned int roomArray[24][5];/*Bitfields representing each char cell on screen.*/

/*The parts of the map array with dependencies resolved. Can be reused after initialisation.*/
unsigned int counted[16];
unsigned int playableArea=0;/*Tracks number of accessible rooms.*/

void
collisionSet(x,y,i)
	char x;
	char y;
	char i;
{
	char bX=x/16;
	char mX=x%16;
	if(i)
		BITSET(roomArray[y][bX],mX);
	else
		BITCLEAR(roomArray[y][bX],mX);

}

char
collisionGet(x,y)
	char x;
	char y;
{
	char bX=x/16;
	char mX=x%16;
	return BITGET(roomArray[y][bX],mX);

}

#ifdef __MSDOS__
int
sp_ticks()
{
	#asm
	xor ax,ax
	int #$1a
	mov ax,dx
	ret
	#endasm
}

char
sp_readchar()
{
	#asm
	mov AH,#$1
	int #$16
	jz nochar
	mov AH,#0
	int #$16
	xor ah,ah
	ret
	nochar:
	xor AX,AX
	ret
	#endasm
}

int
sp_pause()
{
	int start;
	start=sp_ticks();
	while(sp_ticks()<start+2)
	{
		#asm
		hlt
		#endasm
	}
	return start;
}
#endif

#ifdef __unix__
int
sp_readchar()
{
	return getchar();
}
#endif


unsigned int
rule30(state)
	unsigned int state;
{
	unsigned int out=0;
	int i;

	for(i=0;i<16;i++)
	{
		if(BITGET(state,(i-1)%15)^(BITGET(state,i)|BITGET(state,(i+1)%15)))
			BITSET(out,i);

	}
	return out;
}

unsigned int 
rng()
{
  rngState *= 293;
  rngState += 65239;

  return rngState;
}

void
dependencies(x,y,c)
	char x;
	char y;
	int c;
{
	if(x<0 || x>15 || y<0 || y>15) 
		return;
	if(c>100)
	{
		BITCLEAR(worldArray[y],x);
		return;
	}
	if(BITGET(counted[y],x) || !BITGET(worldArray[y],x))
		return;
	

	BITSET(counted[y],x);
	c++;
	playableArea++;
	if(c>20 && c<50) BITSET(strengthArray[y],x);
	else if (c>50 && c<75) BITSET(teleportArray[y],x);
	else if (c>75) BITSET(bombArray[y],x);
	
	dependencies(x,y-1,c);
	dependencies(x+1,y,c);
	dependencies(x,y+1,c);
	dependencies(x-1,y,c);
}

void
initialise()
{
	int i,j;

	worldArray[0]=rng();
	for(i=1;i<16;i++) 
	{
		worldArray[i]=0;
		strengthArray[i]=0;
		teleportArray[i]=0;
		bombArray[i]=0;
		counted[i]=0;
	}

	for(i=0;i<15;i++)
	{
		worldArray[i+1]=rule30(worldArray[i]);
	}
	for(i=0;i<15;i++)/*Fills rooms with a neigbour to the bottom and left.*/
	{
		for(j=1;j<16;j++)
		{
			if(BITGET(worldArray[i],j-1) && BITGET(worldArray[i+1],j))
					BITSET(worldArray[i],j);
		}
	}

}

void
external_walls()
{
	int x,y;
	CLEAR(1);
	for(x=0;x<80;x++)
		for(y=0;y<24;y++)
			collisionSet(x,y,0);

	if(sY-1<0 || !BITGET(counted[sY-1],sX))
	{
		GOTOXY(0,0);
		for(x=0;x<80;x++)
		{
			collisionSet(x,1,1);
			putchar('#');
		}
		putchar('\n');
		
	}
	if(sY+1>15 || !BITGET(counted[sY+1],sX))
	{
		GOTOXY(0,23);
		for(x=0;x<80;x++)
		{
			collisionSet(x,23,1);
			putchar('#');
		}
		putchar('\n');
	}
	if(sX-1<0 || !BITGET(counted[sY],sX-1))
	{
		GOTOXY(0,0);
		for(x=0;x<24;x++)
		{
			collisionSet(1,x,1);

			putchar('#');
			putchar('\n');

		}
	}
	if(sX+1>15 || !BITGET(counted[sY],sX+1))
	{
		GOTOXY(79,0);
		for(x=0;x<24;x++)
		{
			collisionSet(79,x,1);			
			GOTOXY(79,x);
			putchar('#');
		}
		putchar('\n');
	}
}

int
spawn(in)
	entity* in;
{
	in->next=entList;
	entList=in;
	return 0;
}

int
main(argc,argv)
	int argc;
	char** argv;
{
	int i;
	int j;

	#ifdef __unix__
	static struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	newt.c_lflag &= ~ECHO;
	newt.c_cc[VMIN] = 0;
	newt.c_cc[VTIME] = 33;
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	#endif

	initialise();
	while(!BITGET(worldArray[7],7))/*So that the player drops in the centre.*/
		initialise();
	dependencies(7,7,0);/*Resolve ability dependencies from centre of map out.*/
	spawn(construct_player());
	CLEAR(0);

	while(1) 
	{
		auto entity* walk;
		auto entity* prev;
		prev=walk=entList;
		if(!walk)
		{
			printf("Panic: no entities!\n");
			return 1;
		}

		do
		{
			static entity* prev;
			switch(walk->runWhen)
			{
				case RUN_ROOM:
					if(walk->sX != sX && walk->sY != sY)
					{
						if(walk->state)
						{
							free(walk->state);		
						}
						prev->next=walk->next;
						free(walk);
						walk=prev;
					}
					else walk->turn(walk);
					break;
				case RUN_PERSIST:
					if(walk->sX==sX && walk->sY==sY)
						walk->turn(walk);
					break;
				case RUN_ALWAYS:
					walk->turn(walk);
					break;
				case RUN_DIE:
					free(walk->state);
					prev->next=walk->next;
					free(walk);
					walk=prev;
					break;

			}
			prev=walk;
		}
		while(walk=walk->next);
	}	

	return 0;
}
