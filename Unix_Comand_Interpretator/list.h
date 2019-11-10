#ifndef __LIST_H__
#define __LIST_H__
typedef struct Node *link1;
typedef char *elemtype;

typedef struct Node
{
            elemtype elem;
                        link1 next;
}node;

typedef link1 list;

void freeList(list *lst);
void print(list p);
void insback(list *head, char *s);
void addchar(char **c, char sym);
char* itoa(int i, char b[]);
list getList();

#endif

