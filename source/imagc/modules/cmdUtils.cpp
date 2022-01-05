/*
 * cutils.cpp
 *
 *  Created on: Apr 7, 2010
 *      Author: murco
 */
#include "cmdUtils.h"

void gotoxy(int x, int y) {
	printf("\033[%d;%df", y, x);
}

void textcolor(int attr, int fg, int bg)
{	char command[13];
	/* Command is the control command to the terminal */
	//sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	sprintf(command, "%c[%d;%dm", 0x1B, attr, fg + 30);
	printf("%s\n", command);
}

int pressKey(const char *key)
{
	int ch, ret;
	ret = 0;
	changemode(1);
	if(kbhit())
	{
		ch = getchar();
		if(ch==*key)	ret = 1;
	}
	changemode(0);
	return(ret);

}
void changemode(int dir)
{
  static struct termios oldt, newt;

  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void)
{
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);

  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}

void end_program(bool wait){

	if (wait){
		printf("\nPress enter to exit ...\n");
		getchar();
	}

	exit(0);
}
