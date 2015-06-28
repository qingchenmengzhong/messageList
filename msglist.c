#include "msglist.h"

static int singleton = 0; //only one instance may be created at any time

struct node {
    struct node * next;
    char * elem;
    time_t event_time;
};

struct msglist {
    struct node * root;
    size_t len;
};

MessageList createMessageList() {
    if(singleton) //if an instance already exists do not create a new one
        return 0;
    singleton++;
    struct msglist * newlist = calloc(1, sizeof *newlist);
    return newlist;
}

void deleteMessageList() {
    if(mlist == 0)
        return;
    struct msglist * cmlist = (struct msglist *)mlist;

    struct node * tmp = cmlist->root;
    while(tmp != 0) {
        struct node * next = tmp->next;
        free(tmp->elem);
        free(tmp);
        tmp = next;
    }
    free(mlist);
    singleton--;
    mlist = 0;
}


void updateAlarm() {
    if(mlist == 0)
        return;
    struct msglist * cmlist = (struct msglist *)mlist;
    if(cmlist->root == 0)
        return;

    time_t curtime = time(0);
    time_t event = cmlist->root->event_time;
    if(event < curtime)
        messageHandler(SIGALRM);
    else
        alarm((unsigned int)(event - curtime));
}

void selectList(MessageList x) {
    mlist = x;
}

void addmsg(char * newmsg, unsigned int offset) {
    if(mlist == 0 || newmsg == 0 || offset == 0)
        return;
    struct msglist * cmlist = (struct msglist *)mlist;

    time_t newtime = time(0) + offset;
    struct node * tmp = cmlist->root, * prev = 0;
    while(tmp != 0 && tmp->event_time < newtime) { //go to the place where we need to insert the new event
        prev = tmp;
        tmp = tmp->next;
    }

    if(prev == 0) { //create a new node from scratch
        struct node * prev_root = cmlist->root;
        cmlist->root = calloc(1, sizeof *cmlist->root);
        cmlist->root->next = prev_root;
        cmlist->root->elem = malloc(strlen(newmsg)+1);
        strcpy(cmlist->root->elem, newmsg);
        cmlist->root->event_time = newtime;
    } else { //or insert it where it belongs
        struct node * prevnext = prev->next;
        prev->next = calloc(1, sizeof *prev);
        prev->next->next = prevnext;
        prev->next->elem = malloc(strlen(newmsg)+1);
        strcpy(prev->next->elem, newmsg);
        prev->next->event_time = newtime;
    }
    cmlist->len++;
    updateAlarm(); //root may have been replaced so we reset the alarm to the next event in the queue
    return;
}

void printFromNode(struct node * x) {
    if(x == 0) {
        printf("[-]");
        return;
    }
    printf("[%s]->", x->elem);
    printFromNode(x->next);
}


void printList() {
    if(mlist == 0)
        return;
    struct msglist * cmlist = (struct msglist *)mlist;
    printFromNode(cmlist->root);
    printf("\n");
}

char * retrMsg() { //callee must free the data himself
    if(mlist == 0)
        return 0;
    struct msglist * cmlist = (struct msglist *)mlist;
    if(cmlist->root == 0)
        return 0;

    char * ret = cmlist->root->elem;
    struct node * toFree = cmlist->root;
    cmlist->root = cmlist->root->next;
    free(toFree);
    cmlist->len--;
    updateAlarm();
    return ret;
}

time_t timeToNextMsg() {
    if(mlist == 0)
        return time(0);
    struct msglist * cmlist = (struct msglist *)mlist;
    if(cmlist->root == 0)
        return time(0);
    return cmlist->root->event_time;
}

size_t listLen() {
    if(mlist == 0)
        return 0;
    struct msglist * cmlist = (struct msglist *)mlist;
    return cmlist->len;
}

void printState() {
    printf("Status: ");
    printf("%s", status != 0 ? status : "none");
    printf(" - next in %ld sec.\n", timeToNextMsg()-time(0)); //time till next event
}

void setState(char * newstate) {
    if(status != 0) {
        free(status);
    }
    status = newstate;
    printState();
}

int nextIsNow() {
    if(mlist == 0)
        return 0;
    struct msglist * cmlist = (struct msglist *)mlist;
    if(cmlist->root == 0)
        return 0;
    return cmlist->root->event_time <= time(0);
}

void messageHandler(int sig) {
    assert(sig == SIGALRM);
    char * msg = 0;
    do {
        msg = retrMsg();
        setState(msg); //handle event
    } while(nextIsNow());
}
