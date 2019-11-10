#include "list.h"
#include "tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>

list lst = NULL,lst_tmp = NULL;

void print_tree(cmd_tree tree,int space);

void error(char *str);

void make_null(cmd_tree tree)
{
    tree->argv = NULL;
    tree->infile = NULL;
    tree->outfile = NULL;
    tree->bg = 0;
    tree->append = 0;
    tree->nxt = NXT;
    tree->subcmd = NULL;
    tree->pipe = NULL;
    tree->next = NULL;    
}

void makebgrnd(cmd_tree tree);

void bg(cmd_tree tree)
{
    if(tree != NULL)
    {
        tree->bg = 1;
        if(tree->subcmd != NULL)
            bg(tree->subcmd);
        if(tree->pipe != NULL)
            bg(tree->pipe);
        if(tree->next != NULL)
            bg(tree->next);
    }
}

void makebgrnd(cmd_tree tree)
{
    if(tree != NULL)
        tree->bg = 1;
    if(tree->subcmd != NULL)
        bg(tree->subcmd);
    if(tree->pipe != NULL)
        bg(tree->pipe);

}

int next_lex()
{
    lst = lst->next;   
    if(lst != NULL)
        return 1;
    else return 0;
}

cmd_tree com_sh()
{
    if(lst == NULL)
        return NULL;
    else
        return lst_com();    
}

cmd_tree lst_com()
{   
    char *elem;
    cmd_tree tree2, tree1 = conv(),tree_tmp = tree1;
    if(lst == NULL)
        return tree1;
    while( lst != NULL && (strcmp(lst->elem, "&") == 0 ||
                strcmp(lst->elem, "&&") == 0 ||
                strcmp(lst->elem, "||") == 0 || 
                strcmp(lst->elem, ";") == 0) )
    {

        if(lst->next == NULL || strcmp(lst->next->elem, ")") == 0)
        {
            if(strcmp(lst->elem ,"&") ==0)
            {
                makebgrnd(tree_tmp);
                next_lex(); 
                return tree1;
            }
            else if (strcmp(lst->elem, ";") ==0)
            {   
                next_lex();
                return tree1;
            }
            else error("Syntax ERROR: operand not found!");
        }
        elem = lst->elem;
        next_lex();
        tree2 = conv();
        if(strcmp(elem, "&") == 0)
        {   
            makebgrnd(tree_tmp);

        }
        if(strcmp(elem, "&&") == 0)
        {
            tree_tmp->nxt = AND;           
        }
        if(strcmp(elem, "||") == 0)
        {
            tree_tmp->nxt = OR;
        }
        if(strcmp(elem, ";") == 0)
        {   
            tree_tmp->nxt = NXT;
        }

        tree_tmp->next = tree2;
        tree_tmp = tree2;

    }
    return tree1; 
}



cmd_tree conv()
{
    cmd_tree tree1=cmd(),tree2,tree_tmp = tree1;    
    if(lst == NULL)
        return tree1;
    while(lst != NULL && strcmp(lst->elem, "|") == 0)
    {          
        if(lst->next == NULL)
            error("Pipe ERROR: second operand not found!");
        next_lex();
        tree2 = cmd();         
        tree_tmp->pipe = tree2;
        tree_tmp = tree2;

    }
    return tree1;
}

cmd_tree cmd()
{
    cmd_tree tree1; 
    if(lst->next != NULL && strcmp(lst->elem, "(") == 0)
    {
        tree1 = (cmd_tree)malloc(sizeof(struct cmd_inf));
        make_null(tree1);
        next_lex();
        tree1->subcmd = lst_com();        
        if(lst == NULL)
            error("ERROR: ')' Expected");
        if(strcmp(lst->elem, ")") == 0)
            next_lex();
        else 
            error(" ')' excpexted");
        if(lst == NULL)
            return tree1;

        if(strcmp(lst->elem, "<") == 0)
        {   
            next_lex();
            if(lst == NULL)
                error("Argument expected");
            tree1->infile = lst->elem;
            next_lex();

            if(lst == NULL)
                return tree1;
        }

        if(strcmp(lst->elem, ">") == 0)
        {
            next_lex();
            if(lst == NULL)
                error("Argument expected");
            tree1->outfile = lst->elem;
            next_lex();
        }
        else if(strcmp(lst->elem, ">>") == 0)
        {
            next_lex();
            if(lst == NULL)
                error("Argument expected");
            tree1->outfile = lst->elem;
            tree1->append = 1;
            next_lex();
        }

        return tree1;
    }
    else
    {   
        return s_cmd();
    }

}

cmd_tree s_cmd()
{ 
    int i;
    cmd_tree tree1;
    if(lst == NULL)
        error("Command Excpected");
    if(     lst != NULL 
            && strcmp(lst->elem, "<") != 0 
            && strcmp(lst->elem, ">") != 0 
            && strcmp(lst->elem, ">>") != 0
            && strcmp(lst->elem, ")") != 0
            && strcmp(lst->elem, "(") != 0
            && strcmp(lst->elem, "|") != 0
            && strcmp(lst->elem, "||") != 0
            && strcmp(lst->elem, "&") != 0
            && strcmp(lst->elem, "&&") != 0
            && strcmp(lst->elem, ";") != 0)
    {
        tree1 = (cmd_tree)malloc(sizeof(struct cmd_inf));
        make_null(tree1);
        tree1->argv = (char**)malloc(sizeof(char*)*2);
        tree1->argv[0] = lst->elem;
        tree1->argv[1] = NULL;
        next_lex();
    }
    else
        error("Syntax ERROR");   

    i = 3;
    while(lst != NULL 
            && strcmp(lst->elem, "<") != 0 
            && strcmp(lst->elem, ">") != 0 
            && strcmp(lst->elem, ">>") != 0
            && strcmp(lst->elem, ")") != 0
            && strcmp(lst->elem, "|") != 0
            && strcmp(lst->elem, "||") != 0
            && strcmp(lst->elem, "&") != 0
            && strcmp(lst->elem, "&&") != 0
            && strcmp(lst->elem, ";") != 0)
    {        
        tree1->argv = (char**)realloc(tree1->argv,sizeof(char*)*i);
        tree1->argv[i-2] = lst->elem;
        tree1->argv[i-1] = NULL;
        i++;
        next_lex();

    }

    if(lst == NULL)
        return tree1;

    if(strcmp(lst->elem, "<") == 0)
    {   
        next_lex();
        if(lst == NULL)
            error("Argument expected");
        tree1->infile = lst->elem;
        next_lex();

        if(lst == NULL)
            return tree1;
    }

    if(strcmp(lst->elem, ">") == 0)
    {
        next_lex();
        if(lst == NULL)
            error("Argument expected");
        tree1->outfile = lst->elem;
        next_lex();
        if(lst == NULL)
            return tree1;
    }
    else if(strcmp(lst->elem, ">>") == 0)
    {
        next_lex();
        if(lst == NULL)
            error("Argument expected");
        tree1->outfile = lst->elem;
        tree1->append = 1;
        next_lex();
        if(lst == NULL)
            return tree1;
    }    
    return tree1;

}

void print_space(int space)
{
    int i;
    for(i=0;i<space;i++)
        printf(" ");
}

void print_tree(cmd_tree tree, int space)
{
    int i;
    printf("\n");
    if(tree != NULL)
    {
        i=0;
        print_space(space);
        printf("argv: ");
        if(tree->argv == NULL)
            printf("NULL");
          else
        {
            while(tree->argv[i] != NULL)
            {
                printf("%s ",tree->argv[i]);
                i++;
            }
        }
        printf("\n");
        print_space(space);
        printf("infile: %s\n", tree->infile);
        print_space(space);
        printf("outfile: %s\n", tree->outfile);
        print_space(space);
        printf("bg: %d\n", tree->bg);
        print_space(space);
        printf("append: %d\n", tree->append);
        print_space(space); 
        printf("nxt: %d\n", tree->nxt);
        printf("\n");
        if(tree->subcmd != NULL)
        {
            print_space(space);
            printf("//subcmd:\n");
            print_tree(tree->subcmd,space +5);
        }
        if(tree->pipe != NULL)
        {
            print_space(space);
            printf("//pipe:\n");
            print_tree(tree->pipe,space +5);
        }
        if(tree->next != NULL)
        {
            print_space(space);
            printf("//next:\n");
            print_tree(tree->next,space +5);
        }
    }
}

void free_tree(cmd_tree tree)
{
    if(tree == NULL)
        return;
    if(tree->subcmd != NULL)
        free_tree(tree->subcmd);
    if(tree->pipe != NULL)
        free_tree(tree->pipe);
    if(tree->next != NULL)
        free_tree(tree->next);
    if(tree->argv != NULL)
    {
        free(tree->argv);
        
    }
    free(tree);
}

