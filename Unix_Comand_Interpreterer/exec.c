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


int is_com(cmd_tree t)
{
    char *s;
    s = t -> argv[0];
    if (!strcmp(s, "cd") || !strcmp(s, "clear")) {
        return 1;
    }
    return 0;
}

int exec_cd(cmd_tree t)
{
    if (t -> argv[1] == NULL) {
        chdir(getenv("HOME"));
        return 0;
    }
    if (chdir(t -> argv[1]) == -1) {
        fprintf(stderr, "cd: %s: no shuch file or directory\n", t -> argv[1]);
        return 1;
    }
    return 0;
}

int exec_clear()
{
    write(1, "\033[2J\033[H", sizeof("\033[2J\033[H"));
    return 0;
}

int sh_com(cmd_tree t)
{
    char *s;
    s = t -> argv[0];
    if (!strcmp(s, "cd")) {
        return exec_cd(t);
    }
    if (!strcmp(s, "clear")) {
        return exec_clear();
    }
    return 1;
}
void chng_iofiles(int in, int out, cmd_tree t)
{
    int fd;
    if (in != -1) {
        dup2(in, 0);
        close(in);
    }
    if (out != -1) {
        dup2(out, 1);
        close(out);
    }
    if (t -> infile != NULL) {
        if ((fd = open(t -> infile, O_RDONLY)) == -1) {
            printf("\x1B[31m");
            perror("open");
            printf("\x1B[0m");
            exit(1);
        }
        dup2(fd, 0);
        close(fd);
    }
    if (t -> outfile != NULL) {
        fd = open(t -> outfile, O_WRONLY | O_CREAT | (t -> append ? O_APPEND : O_TRUNC), 0666);
        if (fd == -1) {
            printf("\x1B[31m");
            perror("open");
            printf("\x1B[0m");
            exit(1);
        }
        dup2(fd, 1);
        close(fd);
    }
}

void add_elem (intlist **f, int pid) {
    intlist *cur;
    if ((*f) == NULL) {
        *f = (intlist *) malloc(sizeof(intlist));
        cur = *f;
    }
    else {
        cur = *f;
        while (cur -> next != NULL) {
            cur = cur -> next;
        }
        cur -> next = (intlist *) malloc(sizeof(intlist));
        cur = cur -> next;
    }
    cur -> pid = pid;
    cur -> next = NULL;
}

void clear_intlist(intlist *f) {
    intlist *cur;
    cur = f;
    while (cur != NULL) {
        cur = cur -> next;
        free(f);
        f = cur;
    }
}

int exec_com_sh(cmd_tree t)
{
    return exec_com_list(t, 0);
}

int exec_com_list(cmd_tree t, int p)
{
    int ret, to_do;
    ret = exec_conv(t, p);
    while (t -> next != NULL) {
        to_do = 1;
        if (t -> nxt == AND && ret) {
            to_do = 0;
        }
        else if (t -> nxt == OR && !ret) {
            to_do = 0;
        }
        t = t -> next;
        if (to_do) {
            ret = exec_conv(t, p);
        }
    }
    return ret;
}

void print_intlist(intlist *f) {
    while(f != NULL) {
        fprintf(stderr, "%d\n", f -> pid);
        f = f -> next;
    }
}

int exec_conv(cmd_tree t, int p)
{
    int ret, in, out, next_in, i = 0,status;
    int fd[2];
    intlist *frgrnd = NULL;
    if (t -> pipe == NULL) {
        return exec_command(t, -1, -1, -1, p, &frgrnd);
    }
    next_in = -1;
    while (t -> pipe != NULL) {
        i++;
        pipe(fd);
        in = next_in;
        next_in = fd[0];
        out = fd[1];
        ret = exec_command(t, in, out, next_in, 1, &frgrnd);
        close(in);
        close(out);
        t = t -> pipe;
    }
    ret = exec_command(t, next_in, -1, -1, 1, &frgrnd);
    close(next_in);
    while(frgrnd != NULL) {
        int s = waitpid(frgrnd -> pid, &status, 0);
        frgrnd = frgrnd -> next;
    }
    clear_intlist(frgrnd);
    return ret;
}

int exec_command(cmd_tree t, int in, int out, int next_in, int p, intlist **pfrgrnd)
{
    int  status, pid;
    if (t -> subcmd == NULL) {
        return exec_simple_com(t, in, out, next_in, p, pfrgrnd);
    }
    if ((pid = fork()) == 0) {
        clear_intlist(*pfrgrnd);
        chng_iofiles(in, out, t);
        if (next_in != -1) {
            close(next_in);
        }
        exec_com_list(t->subcmd,0);
        exit(0);
    }
    else {
        if (t->bg) {
            add_elem(&bckgrnd, pid);
            waitpid(pid, &status, WNOHANG);
            if (WIFEXITED(status)) {
                 return WEXITSTATUS(status);
            }
            return -1;
            
            
        }
        waitpid(pid,&status,0);
        return WEXITSTATUS(status);
        
    }
}

int exec_simple_com(cmd_tree t, int in, int out, int next_in, int p, intlist **pfrgrnd)
{
    int status, pid;
    if (is_com(t)) {
        return sh_com(t);
    }
    if ((pid = fork()) == 0) {
        signal(SIGINT, oldHandler);
        chng_iofiles(in, out, t);
        close(next_in);
        execvp(t -> argv[0], t -> argv);
        printf("\x1B[31m");
        perror(t -> argv[0]);
        printf("\x1B[0m");
        exit(1);
    }
    if (t -> bg) {
        waitpid(pid, &status, WNOHANG);
        add_elem(&bckgrnd, pid);
        zombie++;
    }
    else if (p) {
        add_elem(pfrgrnd, pid);
        return 0;
    }
    else {
       waitpid(pid, &status, 0);
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } 
    return 1;
}

void clear_zombie(intlist *b)
{
    intlist *p = b;
    int pid, status;
    b = b -> next;
    while (b != NULL) {
        pid = waitpid(b -> pid, &status, WNOHANG);
        if (pid != 0) {
            p->next = b->next;
            free(b);
            b = p ->next;
        }
        else {
            p = b;
            b = b -> next;
        }
    }
}

void exec_tree(cmd_tree tr)
{
    exec_com_sh(tr);
}
