#ifndef __TREE_H__
#define __TREE_H__

list lst,lst_tmp;
enum typenxt {NXT,AND,OR};

typedef struct cmd_inf *cmd_tree;
struct cmd_inf
{
    char **argv;
    char *infile;
    char *outfile;
    int bg;
    int append;
    enum typenxt nxt;
    cmd_tree subcmd;
    cmd_tree pipe;
    cmd_tree next;
};

cmd_tree com_sh();
cmd_tree lst_com();
cmd_tree conv();
cmd_tree cmd();
cmd_tree s_cmd();

void print_tree(cmd_tree tree,int space);

void error(char *str);

void make_null(cmd_tree tree);
void makebgrnd(cmd_tree tree);
void bg(cmd_tree tree);
int next_lex();
void print_space(int space);
void print_tree(cmd_tree tree, int space);
void free_tree(cmd_tree tree);

#endif
