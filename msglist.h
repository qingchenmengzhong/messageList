#ifndef MSGLIST_H_INCLUDED
#define MSGLIST_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

typedef void * MessageList;

static MessageList mlist = 0; //currently selected list (there may only be one at a time)

MessageList createMessageList(void);
void deleteMessageList(void);
void addmsg(char * newmsg, unsigned int offset);
void selectList(MessageList);
void printList(void);

time_t timeToNextMsg(void);
size_t listLen(void);

static char * status = 0;
void printState(void);
void setState(char * newstate);

void messageHandler(int sig);

#endif // MSGLIST_H_INCLUDED
