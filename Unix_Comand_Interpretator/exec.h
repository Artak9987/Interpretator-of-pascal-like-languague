#include <stdlib.h>
#ifndef __EXEC_H__
#define __EXEC_H__

typedef struct intList
{
    int pid;
    struct intList *next;
} intlist;

extern intlist *bckgrnd;
extern int zombie;
extern void (*oldHandler)(int);

int exec_com_sh(cmd_tree);
int exec_com_list(cmd_tree, int);
int exec_conv(cmd_tree, int);
int exec_command(cmd_tree, int, int ,int, int, intlist **);
int exec_simple_com(cmd_tree, int, int, int, int, intlist **);
void background(cmd_tree);
int is_com(cmd_tree t);
int exec_cd(cmd_tree t);
int exec_clear();
int sh_com(cmd_tree t);
void chng_iofiles(int in, int out, cmd_tree t);
void add_elem (intlist **f, int pid);
void clear_intlist(intlist *f);
void print_intlist(intlist *f);
void clear_zombie(intlist *b);
void exec_tree(cmd_tree tr);

#endif
