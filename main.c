#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "msglist.h"

#define BUFFER_LENGTH 200
#define BUFFER_LENGTH_LITERAL "200"

int main()
{
    struct sigaction act;
    memset(&act, '\0', sizeof act);

    act.sa_handler = messageHandler; //use simple signal handler
    //act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGALRM, &act, NULL) < 0) { //hook SIGALRM to call our messageHandler function
        perror("sigaction");
        exit(1);
    }

    printf("Use <seconds> <message>\n");

    selectList(createMessageList()); //create signleton message list
    unsigned int offset = 1;
    char * buf = calloc(BUFFER_LENGTH, sizeof *buf); //initialize user input buffer
    while(offset != 0) {
        //fflush(stdin);
        if(scanf("%d %"BUFFER_LENGTH_LITERAL"[0-9a-zA-Z ]", &offset, buf) < 2) //if we were interrupted this would not allow invalid data do be added
            continue;
        addmsg(buf, offset);
    }
    free(buf);
    deleteMessageList();
}


