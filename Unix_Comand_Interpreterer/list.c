#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node *link1;
typedef char *elemtype;

typedef struct Node
{
    elemtype elem;
    link1 next;
}node;

typedef link1 list;

void freeList(list *lst)
{
    list tmp;

    while(*lst != NULL)
    {
        tmp = *lst;
        free(tmp->elem);
        *lst = (*lst)->next;
        free(tmp);
    }
    *lst = NULL;
}

void print(list p)
{
    while(p!=NULL)
    {
        printf("%s ", p->elem);
        p=p->next;
    }
    putchar('\n');
}

void insback(list *head, char *s)
{
    list tmp;
    int len;
    len = strlen(s);

    if(*head == NULL)
    {
        *head = (list)malloc(sizeof(node));
        (*head)->next = NULL;
        (*head)->elem = (char*)malloc(sizeof(char)*(len));
        strcpy((*head)->elem,s);
        return;
    }

    tmp=*head;
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = (list)malloc(sizeof(node));
    tmp->next->elem = (char*)malloc(sizeof(char)*(len));
    strcpy(tmp->next->elem,s);
    tmp->next->next = NULL;
}

int size=0;
addchar(char **c, char sym)
{
    size++;
    *c = (char *)realloc(*c, sizeof(char)*size);
    *(*c+size-1) = sym;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int is_special(char *c)
{
    
    
    if(strcmp(c,"|") != 0 && strcmp(c, "&") != 0 && strcmp(c, ";") != 0 
            && strcmp(c, ">") !=0 && strcmp(c, "<")!=0 && strcmp(c ,"(") != 0 
            && strcmp(c, ")") !=0 && strcmp(c, "&&")!=0 && strcmp(c, "||")!=0 && strcmp(c, ">>")!=0)
    {
        return 0;
    }
    else return 1;
}

list getList()
{
    list lst=NULL;
    char euid[100],user[100];
    itoa(geteuid(),euid);
    enum vertex{start,stop,word,newline,ssymb,ssymb2}
    v = start;
    char *sym = NULL;
    char symb,c;
    while(1)
    {
        if(v != newline && v != stop)
            c = getchar();
        switch(v)
        {
            case start:
                if(c != '|' && c != '&' && c != ';' && c != '>' && c != '<' && c != '(' && c != ')' && c != '\n' && c != ' ' && c != '\t' && c != EOF)
                {
                    addchar(&sym,c);
                    v = word;
                }
                else
                    if(c == ' ' || c == '\t')
                    {
                        v = start;
                    }
                    else
                        if(c == '\n')
                        {
                            insback(&lst,"");
                            v = newline;
                        }
                        else
                            if(c == EOF)
                            {

                                v = stop;
                            }
                            else
                            {
                                addchar(&sym,c);
                                symb = c;
                                v = ssymb;
                            }
                break;

            case word:
                if(c != '|' && c != '&' && c != ';' && c != '>' && c != '<' && c != '(' && c != ')' && c != '\n' && c != ' ' && c != '\t' && c != EOF)
                {
                    addchar(&sym,c);
                    v = word;
                }
                else
                    if(c == ' ' || c == '\t')
                    {
                        addchar(&sym,'\0');
                        if(strcmp(sym,"$HOME") == 0)
                            insback(&lst,getenv("HOME"));
                        else
                            if(strcmp(sym,"$SHELL") == 0)
                                insback(&lst,getenv("SHELL"));
                            else
                                if(strcmp(sym,"$EUID") == 0)
                                    insback(&lst,euid);
                                else
                                    if(strcmp(sym,"$USER") == 0)
                                        insback(&lst,getlogin());
                                    else
                                        insback(&lst,sym);
                        size = 0;
                        v = start;
                    }
                    else
                        if(c == '\n')
                        {
                            addchar(&sym,'\0');
                            if(strcmp(sym,"$HOME") == 0)
                                insback(&lst,getenv("HOME"));
                            else
                                if(strcmp(sym,"$SHELL") == 0)
                                    insback(&lst,getenv("SHELL"));
                                else
                                    if(strcmp(sym,"$EUID") == 0)
                                        insback(&lst,euid);
                                    else
                                        if(strcmp(sym,"$USER") == 0)
                                            insback(&lst,getlogin());
                                        else
                                            insback(&lst,sym);
                            size = 0;
                            v = newline;
                        }
                        else
                            if(c == EOF)
                            {
                                addchar(&sym,'\0');
                                if(strcmp(sym,"$HOME") == 0)
                                    insback(&lst,getenv("HOME"));
                                else
                                    if(strcmp(sym,"$SHELL") == 0)
                                        insback(&lst,getenv("SHELL"));
                                    else
                                        if(strcmp(sym,"$EUID") == 0)
                                            insback(&lst,euid);
                                        else
                                            if(strcmp(sym,"$USER") == 0)
                                                insback(&lst,getlogin());
                                            else
                                                insback(&lst,sym);
                                size = 0;
                                v = stop;
                            }
                            else
                            {
                                addchar(&sym,'\0');
                                if(strcmp(sym,"$HOME") == 0)
                                    insback(&lst,getenv("HOME"));
                                else
                                    if(strcmp(sym,"$SHELL") == 0)
                                        insback(&lst,getenv("SHELL"));
                                    else
                                        if(strcmp(sym,"$EUID") == 0)
                                            insback(&lst,euid);
                                        else
                                            if(strcmp(sym,"$USER") == 0)
                                                insback(&lst,getlogin());
                                            else
                                                insback(&lst,sym);
                                size = 0;
                                addchar(&sym,c);
                                symb = c;
                                v = ssymb;
                            }
                break;

            case ssymb:
                if(c != '|' && c != '&' && c != ';' && c != '>' && c != '<' && c != '(' && c != ')' && c != '\n' && c != ' ' && c != '\t' && c != EOF)
                {
                    addchar(&sym,'\0');
                    insback(&lst,sym);
                    size = 0;
                    addchar(&sym,c);
                    v = word;
                }
                else
                    if(c == ' ' || c == '\t')
                    {
                        addchar(&sym,'\0');
                        insback(&lst,sym);
                        size = 0;
                        v = start;
                    }
                    else
                        if(c == '\n')
                        {
                            addchar(&sym,'\0');
                            insback(&lst,sym);
                            size = 0;
                            v = newline;
                        }
                        else
                            if(c == EOF)
                            {
                                addchar(&sym,'\0');
                                insback(&lst,sym);
                                size = 0;
                                v = stop;
                            }
                            else
                                if(c == symb && c != '<' && c != ';' && c != '(' && c != ')')
                                {
                                    addchar(&sym,c);
                                    v = ssymb2;
                                }
                                else
                                {
                                    addchar(&sym,'\0');
                                    insback(&lst,sym);
                                    size = 0;
                                    addchar(&sym,c);
                                    symb = c;
                                    v = ssymb;

                                }
                break;

            case newline:
                free(sym);
                return lst;
                break;

            case ssymb2:
                if(c != '|' && c != '&' && c != ';' && c != '>' && c != '<' && c != '(' && c != ')' && c != '\n' && c != ' ' && c != '\t' && c != EOF)
                {
                    addchar(&sym,'\0');
                    insback(&lst,sym);
                    size = 0;
                    addchar(&sym,c);
                    v = word;
                }
                else
                    if(c == ' ' || c == '\t')
                    {
                        addchar(&sym,'\0');
                        insback(&lst,sym);
                        size = 0;
                        v = start;
                    }
                    else
                        if(c == '\n')
                        {
                            addchar(&sym,'\0');
                            insback(&lst,sym);
                            size = 0;
                            v = newline;
                        }
                        else
                            if(c == EOF)
                            {
                                addchar(&sym,'\0');
                                insback(&lst,sym);
                                size = 0;
                                v = stop;
                            }
                            else
                            {
                                addchar(&sym,'\0');
                                insback(&lst,sym);
                                size = 0;
                                addchar(&sym,c);
                                symb = c;
                                v = ssymb;
                            }
                break;
            case stop:
                free(sym);
                freeList(&lst);
                lst = NULL;
                return lst;
        }
    }
}

