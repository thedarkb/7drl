extern unsigned int rngState;

/*World data occupying 128 bytes.*/
extern unsigned int worldArray[16];/*The dungeon as 16 bit fields.*/
extern unsigned int strengthArray[16];/*The rooms requiring power bracelet to traverse.*/
extern unsigned int teleportArray[16];/*The rooms requiring teleportation to traverse.*/
extern unsigned int bombArray[16];/*The rooms requiring bombs to traverse.*/

extern unsigned char roomArray[25][80];/*The room currently occupied by the player. 2K(hefty)*/

/*The parts of the map array with dependencies resolved. Can be reused after initialisation.*/
extern unsigned int counted[16];
extern unsigned int playableArea;/*Tracks number of accessible rooms.*/

extern unsigned int sX,sY;
