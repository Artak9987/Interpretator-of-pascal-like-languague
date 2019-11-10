#include "list.h"
#include "tree.h"
#include "exec.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

jmp_buf begin;

void error(char *str)
{
    printf("\x1B[31m");
    printf("%s\n",str);
    printf("\x1B[0m");
    longjmp(begin,1);
}
intlist *bckgrnd = NULL;
int zombie = 0;
void (*oldHandler)(int);

int main()
{
    bckgrnd = (intlist*)malloc(sizeof(intlist));
    bckgrnd->pid = 123;
    bckgrnd -> next = NULL;
    int stat;
    cmd_tree tree = NULL;
    oldHandler = signal(SIGINT, SIG_IGN);
    while(1)
    {
        setjmp(begin);
        free_tree(tree);
        freeList(&lst_tmp);
        clear_zombie(bckgrnd);
        printf(">>");
        lst = getList();
        lst_tmp = lst;
        if(lst == NULL)
            break;
        if(strcmp(lst_tmp->elem, "") == 0)
        {
            tree = NULL;
        }
        else
        {
            tree = com_sh();
            if(lst != NULL)
                error("ERROR: extra lex");
            exec_tree(tree);
        }

    }
    free(bckgrnd);
    printf("\n");
    return WEXITSTATUS(stat);
}
