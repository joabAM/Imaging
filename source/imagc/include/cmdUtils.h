/*
 * cutils.h
 *
 *  Created on: Apr 7, 2010
 *      Author: murco
 */

#ifndef CUTILS_H_
#define CUTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

void gotoxy( int x, int y);
void textcolor(int attr, int fg, int bg);
int pressKey(const char *key);
void changemode(int);
int  kbhit(void);
void end_program(bool wait);

#endif /* CUTILS_H_ */
