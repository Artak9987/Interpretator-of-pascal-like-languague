/* Repeat Until, While do , If then else */
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stack>

using namespace std;

enum type_of_lex
{
    LEX_NULL, /*0*/
    LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT, /*9*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE, LEX_REPEAT, LEX_UNTIL, /*20*/
    LEX_FIN, /*21*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, /*29*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, /*37*/
    LEX_NUM, /*38*/
    LEX_ID, /*39*/
    POLIZ_LABEL, /*40*/
    POLIZ_ADDRESS, /*41*/
    POLIZ_GO, /*42*/
    POLIZ_FGO/*43*/
};

/////////////////////////  Класс Lex  //////////////////////////

class Lex
{
    type_of_lex t_lex;
    int v_lex;
    public:
    Lex ( type_of_lex t = LEX_NULL, int v = 0): t_lex (t), v_lex (v) {}
    type_of_lex  get_type ()  { return t_lex; }
    int          get_value () { return v_lex; }
    friend ostream &    operator<< (ostream &s, Lex l)
    {
        s << '(' << l.t_lex << ',' << l.v_lex << ");" ;
        return s;
    }
};

/////////////////////  Класс Ident  ////////////////////////////

class Ident
{
    char       * name;
    bool         declare;
    type_of_lex  type;
    bool         assign;
    int          value;
    public:
    Ident() { declare = false; assign = false; }
    char       * get_name () { return name; }
    void         put_name (const char *n)
    {
        name = new char [ strlen(n)+1];
        strcpy(name,n);
    }
    bool         get_declare () { return declare; }
    void         put_declare () { declare = true; }
    type_of_lex  get_type    () { return type; }
    void         put_type    ( type_of_lex t ) { type = t; }
    bool         get_assign  () { return assign; }
    void         put_assign  (){ assign = true; }
    int          get_value   () { return value; }
    void         put_value   (int v){ value = v; }
};

//////////////////////  Класс Tabl_ident  ///////////////////////

class Tabl_ident
{
    Ident      * p;
    int          size;
    int          top;
    public:
    Tabl_ident ( int max_size )
    {
        p = new Ident [ size = max_size ];
        top = 1;
    }
    ~Tabl_ident () { delete [] p; }
    Ident      & operator[] ( int k ) { return p[k]; }
    int          put ( const char *buf );
};

int Tabl_ident::put ( const char *buf )
{
    for ( int j = 1; j < top; j++ )
        if ( !strcmp ( buf, p[j].get_name() ) )
            return j;
    p[top].put_name(buf);
    ++top;
    return top-1;
}

/////////////////////////////////////////////////////////////////

Tabl_ident TID ( 100 );

/////////////////////  Класс Scanner  //////////////////////////////

class Scanner
{
    //enum         state { H, IDENT, NUMB, COM, ALE, DELIM, NEQ };
    static char       * TW    [];
    static type_of_lex  words [];
    static char       * TD    [];
    static type_of_lex  dlms  [];
    FILE       * fp;
    char         c;
    char         buf [ 80 ];
    int          buf_top;
    int d, j;
    Lex L;
    bool stop;
    typedef void (Scanner::*State)();
    State state = nullptr;

    void         clear ()
    {
        buf_top = 0;
        for ( int j = 0; j < 80; j++ )
            buf[j] = '\0';
    }
    void         add ()
    {
        buf [ buf_top++ ] = c;
    }
    int          look ( const char *buf, char **list )
    {
        int i = 0;
        while (list[i])
        {
            if ( !strcmp(buf, list[i]) )
                return i;
            ++i;
        }
        return 0;
    }
    void         gc ()
    {
        c = fgetc (fp);
    }
    void H();
    void IDENT();
    void NUMB();
    void COM();
    void ALE();
    void DELIM();
    void NEQ();
    public:
    Scanner ( const char * program )
    {
        fp = fopen ( program, "r" );
        state = &Scanner::H;
        clear();
        gc();
    }
    Lex          get_lex ();
};

char *
Scanner::TW    [] = {"", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
    "read", "then", "true", "var", "while", "write", "repeat", "until", NULL};

type_of_lex
Scanner::words [] = {LEX_NULL, LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE, LEX_REPEAT, LEX_UNTIL, LEX_NULL};

char *
Scanner::TD    [] = {"", "@", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL};

type_of_lex
Scanner::dlms  [] = {LEX_NULL, LEX_FIN, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ,
    LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_NULL};

void Scanner::H() {
    if ( c==' ' || c == '\n' || c== '\r' || c == '\t' ) 
        gc();
    else if ( isalpha(c) )
    {
        clear();
        add();
        gc();
        state = &Scanner::IDENT; 
    }
    else if ( isdigit(c) )
    {
        d = c - '0';
        gc();
        state = &Scanner::NUMB; 
    }
    else if ( c== '{' )
    {
        gc();
        state = &Scanner::COM; 
    }
    else if ( c== ':' || c== '<' || c== '>' )
    { 
        clear(); 
        add(); 
        gc(); 
        state = &Scanner::ALE; 
    }
    else if (c == '@') {
        stop = true;
        L = Lex(LEX_FIN);
    }
    else if (c == '!')
    {
        clear();
        add();
        gc();
        state = &Scanner::NEQ; 
    }
    else 
        state = &Scanner::DELIM; 
}

void Scanner::IDENT() {
    if ( isalpha(c) || isdigit(c) ) 
    {
        add(); 
        gc();
    }
    else if ( j = look (buf, TW) ) {
        stop = true;
        L = Lex (words[j], j);
    }
    else{
    
        j = TID.put(buf);
        stop = true;
        L = Lex (LEX_ID, j);
    }

}

void Scanner::NUMB() {
    if ( isdigit(c) ) 
    {
        d = d * 10 + (c - '0');
        gc();
    }
    else {
        stop = true;
        L = Lex ( LEX_NUM, d);
    }
}

void Scanner::COM() {
    if ( c == '}' )
    {
        gc();
        state = &Scanner::H; 
    }
    else if (c == '@' || c == '{' )
        throw c;
    else
        gc();
}

void Scanner::ALE() {
    if ( c== '=')
    {
        add();
        gc();
        j = look ( buf, TD );
        stop = true;
        L = Lex ( dlms[j], j);
    }
    else
    {
        j = look ( buf, TD );
        stop = true;
        L = Lex ( dlms[j], j );
    }
}

void Scanner::DELIM() {
    clear();
    add();
    if ( j = look ( buf, TD) )
    {
        gc();
        stop = true;
        L = Lex ( dlms[j], j );
    }
    else
        throw c;
}

void Scanner::NEQ() {
    if (c == '=')
    {
        add();
        gc();
        j = look ( buf, TD );
        stop = true;
        L = Lex ( LEX_NEQ, j );
    }
    else
        throw '!';

}

Lex Scanner::get_lex () 
{
    stop = false;
    state = &Scanner::H;
    while(!stop) {
        (this->*state)();
    }
    return L;  
}

///////////////////////////  Класс Poliz  /////////////////////////////////

class Poliz
{
    Lex        * p;
    int          size;
    int          free;
    public:
    Poliz (int max_size)
    {
        p = new Lex [size = max_size];
        free = 0;
    }
    ~Poliz() { delete [] p; }
    void         put_lex ( Lex l )
    {
        p [free] = l;
        free++;
    }
    void         put_lex ( Lex l, int place) { p [place] = l; }
    void         blank    () { free++; }
    int          get_free () { return free; }
    Lex        & operator[] (int index)
    {
        if ( index > size )
            throw "POLIZ:out of array";
        else
            if ( index > free )
                throw "POLIZ:indefinite element of array";
            else
                return p[index];
    }
    void         print ()
    {
        for ( int i = 0; i < free; ++i )
            cout << p[i]<<endl;

    }
};

///////////////////////////  Класс Parser  /////////////////////////////////

class Parser 
{
    enum vertex {Pbeg, Pend, P1, P2, P3, P4, Dbeg, D1, D2, Dend, Ybeg,/*10*/
        Y1, Yend, Bbeg, B1, B2, Bend, Sbeg, Send, S1, S2,/*20*/ 
        S3, S4, S5, S6, S7, S8, S9, S10, S11, S12,/*30*/
        S13, S14, S15, S16,S17,S18,S19, Tbeg, T1, Tend,/*40*/ 
        Xbeg, Xend, X1, Ebeg , Eend, E1, E2, Fbeg, Fend, F1, F2, F3/*52*/};
    Lex          curr_lex;
    type_of_lex  c_type;
    int          c_val;
    Scanner      scan;
    stack < int > st_int;
    stack < type_of_lex >  st_lex;
    stack < string > st_l;
    void         check_id_in_read ();
    void         gl ()
    {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }
    void         dec ( type_of_lex type);
    void         check_id ();
    void         check_op ();
    void         check_not ();
    void         eq_type ();
    void         eq_bool ();
    public:
    Poliz prog;
    Parser (const char *program ) : scan (program),prog(1000) {}
    void         analyze();
    struct temp{
        int pl0;
        int pl1;
    };
    temp tmp;
    stack < temp > tmp_st;


};


class Executer
{
    Lex          pc_el;
    public:
    void         execute ( Poliz & prog );
};


void Executer::execute ( Poliz & prog )
{
    stack < int > args;
    int i, j, arg, arg1, index = 0, size = prog.get_free();
    while ( index < size )
    
    {
//        if (!args.empty()) cerr<<"top " <<args.top();
        pc_el = prog [ index ];
        switch ( pc_el.get_type () )
        {
            case LEX_TRUE:
                args.push(1);
                break;
            case LEX_FALSE:
                args.push(0);
                break;
            case LEX_NUM: case POLIZ_ADDRESS: 
            case POLIZ_LABEL:
                
        //        cerr<<"lexnum = "<<pc_el.get_value();

                args.push ( pc_el.get_value () );
                break;
            case LEX_ID:
                i = pc_el.get_value ();
                if ( TID[i].get_assign () )
                {
                    args.push ( TID[i].get_value () );
                    break;
                }
                else
                    throw "POLIZ: indefinite identifier";
            case LEX_NOT:
                arg = !args.top();
                args.pop();
                args.push( arg );
                break;
            case LEX_OR:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg || i );
                break;
            case LEX_AND:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg && i );
                break;
            case POLIZ_GO:

                arg = args.top();
                args.pop();
                index = arg - 1;
                break;
            case POLIZ_FGO:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                if ( !arg ) index = i-1;
                break;
            case LEX_WRITE:
                arg = args.top();
                args.pop();
                cout << arg << endl;
                break;
            case LEX_READ:
                {
                    int k;
                    i = args.top ();
                    args.pop();
                    if ( TID[i].get_type () == LEX_INT )
                    {
                        cout << "Input int value for" << TID[i].get_name () << endl;
                        cin >> k;
                    }
                    else
                    {
                        char j[20];
rep:
                        cout << "Input boolean value (true or false) for" << TID[i].get_name() << endl;
                        cin >> j;
                        if (!strcmp(j, "true"))
                            k = 1;
                        else
                            if (!strcmp(j, "false"))
                                k = 0;
                            else
                            {
                                cout << "Error in input:true/false" << endl;
                                goto rep;
                            }
                    }
                    TID[i].put_value (k);
                    TID[i].put_assign ();
                    break;
                }
            case LEX_PLUS:
                arg = args.top();
                args.pop();
                arg1 = args.top();
                args.pop();
                args.push ( arg + arg1 );
                break;
            case LEX_TIMES:
                arg = args.top();
                args.pop();
                arg1 = args.top();
                args.pop();
                args.push ( arg * arg1 );
                break;
            case LEX_MINUS:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg - i );
                break;
            case LEX_SLASH:
                i = args.top();
                args.pop();
                //cerr<<"  i = "<<i<<endl;
               // cerr<<"  !i = "<<!i<<endl;
                if (i)
                {
                    arg = args.top();
                    args.pop();
                    args.push(arg / i);
                    break;
                }
                else
                    throw "POLIZ:divide by zero";
                break;
            case LEX_EQ:
                arg = args.top();
                args.pop();
                arg1 = args.top();
                args.pop();
                args.push ( arg == arg1 );
                break;
            case LEX_LSS:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg < i);
                break;
            case LEX_GTR:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg > i );
                break;
            case LEX_LEQ:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg <= i );
                break;
            case LEX_GEQ:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg >= i );
                break;
            case LEX_NEQ:
                i = args.top();
                args.pop();
                arg = args.top();
                args.pop();
                args.push ( arg != i );
                break;
            case LEX_ASSIGN:
                i = args.top();
                args.pop();
                j = args.top();
                args.pop();
                TID[j].put_value(i);
            //    cerr<<" j = "<<j<<endl;
                TID[j].put_assign(); 
              //  cerr<<"put assign"<<TID[j].get_assign()<<endl;
                break;
            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    cout << "Finish of executing!!!" << endl;
}


class Interpretator
{
    // Parser   pars;
    Executer E;
    public:
    Parser pars;
    Interpretator  (char* program): pars (program) {}
    void     interpretation ();
};

void Interpretator::interpretation ()
{
    pars.analyze ();
    E.execute ( pars.prog );
}


void Parser::analyze ()
{
    gl();
    st_l.push("(st");
    vertex v = Pbeg;
    do {
        //       cerr<<' '<<v<<' ';
        switch (v) {
            case Pbeg:
                //st_l.push("(start");
                if (c_type == LEX_PROGRAM) 
                {
                    gl();
                    v = P1;
                }
                else {
                    throw "Program expected";
                }
                break;
            case P1:
                st_l.push("(PY");
                v = Ybeg;
                break;
            case Ybeg:
                if (c_type == LEX_VAR) {
                    gl();
                    v = Y1;
                } 
                else {
                    throw "Var expected";
                }                   
                break;
            case Y1:
                st_l.push("(YD");
                v = Dbeg;
                break;

            case Dbeg:
                if (c_type == LEX_ID)
                {
                    st_int.push(c_val);
                    gl();
                    v = D1;
                }
                else {
                    throw "Identifier expected";
                }
                break;
            case D1:
                if (c_type == LEX_COMMA)
                {
                    gl();
                    v = Dbeg;
                }
                else if (c_type == LEX_COLON)
                {
                    gl();
                    v = D2;
                }
                else
                    throw "Invalid defenition of variables";
                break;


            case D2:
                if (c_type == LEX_INT ) 
                {
                    dec(LEX_INT);
                    gl();
                    v = Dend;
                }
                else if (c_type == LEX_BOOL)
                {
                    dec(LEX_BOOL);
                    gl();
                    v = Dend;
                }
                else 
                    throw "Typename expected";
                break;
            case Dend:
                if (st_l.top() == "(YD")
                {
                    st_l.pop();
                    v = Yend;
                }
                else
                    throw "Invalid defenition";
                break;
            case Yend:
                if (c_type == LEX_COMMA)
                {
                    gl();
                    v = Y1;
                }
                else if (st_l.top() == "(PY") 
                {
                    st_l.pop();
                    v = P2;
                }
                else
                    throw "Variable section invalid";
                break;
            case P2:
                if (c_type == LEX_SEMICOLON)
                {
                    gl();
                    v = P3;
                }
                else 
                    throw "Semocolon expected";
                break;
            case P3:
                st_l.push("(PB");
                v = Bbeg;
                break;
            case P4:
                if (c_type == LEX_FIN) 
                {
                    gl();
                    v = Pend;
                }
                else
                    throw "End of program expected";
                break;
            case Bbeg:
                if (c_type == LEX_BEGIN)
                {
                    gl();
                    v = B1;
                }
                else 
                    throw "Begin expected";
                break;
            case B1:
                st_l.push("(BS");
                v = Sbeg;
                break;
            case B2:
                if (c_type == LEX_SEMICOLON)
                {
                    gl();
                    v = B1;
                }   
                else if (c_type == LEX_END)
                {
                    gl();
                    v = Bend;
                }
                else
                    throw "Semicolon or end expected";
                break;
            case Bend:
                if (st_l.top() == "(PB")
                {
                    st_l.pop();
                    v = P4;
                }
                else if (st_l.top() == "(SB")
                {
                    st_l.pop();
                    v = Send;
                }
                else 
                    throw "ERROR";
                break;
            case Sbeg:
                if (c_type == LEX_ID)
                {
                    check_id();
                    prog.put_lex(Lex(POLIZ_ADDRESS,c_val));
                    gl();
                    v = S1;
                }
                else if(c_type == LEX_IF) 
                {
                    gl();
                    v = S3;   
                }   
                else if (c_type == LEX_WHILE)
                {
                    gl();
                    v = S8;
                }
                else if (c_type == LEX_READ)
                {
                    gl();
                    v = S11;
                }
                else if (c_type == LEX_WRITE)
                {
                    gl();
                    v = S14;   
                }
                else if(c_type == LEX_REPEAT)
                {
                    tmp.pl0 = prog.get_free();
                    gl();
                    v = S17;
                }
                else if(c_type == LEX_UNTIL)
                {
                    gl();
                    v = S19;
                }
                else 
                {
                    st_l.push("(SB");
                    v = Bbeg;
                }
                break;
            case S1:
                if (c_type == LEX_ASSIGN) 
                {

                    gl();
                    v = S2;
                }
                else 
                    throw "Assign expected";
                break;      
            case S2:
                st_l.push("(SE1");
                v = Ebeg;
                break;
            case S3:
                st_l.push("(SE2");
                v = Ebeg;
                break;
            case S4:
                if (c_type == LEX_THEN)
                {
                    gl();
                    v = S5;    
                }
                else 
                    throw "Then expected";
                break;
            case S5:
                st_l.push("(SS1");
                tmp_st.push(tmp);
                v = Sbeg;
                break;
            case S6:
                if (c_type == LEX_ELSE) {
                    gl();
                    v = S7;
                }
                else 
                    throw "Else expected";
                break;
            case S7:
                st_l.push("(SS2");
                tmp_st.push(tmp);
                v = Sbeg;
                break;
            case S8:
                st_l.push("(SE3");
                tmp.pl0 = prog.get_free();
                v = Ebeg;
                break;
            case S9:
                if (c_type == LEX_DO)
                {
                    gl();
                    v = S10;
                }
                else 
                    throw "Do expected";
                break;
            case S10:
                st_l.push("(SS3");
                tmp_st.push(tmp);
                v = Sbeg;
                break;
            case S11:
                if (c_type == LEX_LPAREN)
                {
                    gl();
                    v = S12;
                }
                else
                    throw "Left paren expected";
                break;
            case S12:
                if (c_type == LEX_ID)
                {
                    check_id_in_read();
                    prog.put_lex(Lex(POLIZ_ADDRESS,c_val));
                    gl();
                    v = S13;
                }
                else 
                    throw "Identificator expected";
                break;
            case S13:
                if (c_type == LEX_RPAREN)
                {
                    prog.put_lex(Lex(LEX_READ));
                    gl();
                    v = Send;
                }
                else 
                    throw "Right paren expected";
                break;
            case S14:
                if (c_type == LEX_LPAREN) 
                {
                    gl();
                    v = S15;
                }
                else
                    throw "Left paren expected";
                break;
            case S15:
                st_l.push("(SE4");
                v = Ebeg;
                break;
            case S16:
                if (c_type == LEX_RPAREN)
                {
                    prog.put_lex(Lex(LEX_WRITE));
                    gl();
                    v = Send;
                }
                else                 
                    throw " ')' EXPECTED";
                break;
            case S17:
                st_l.push("(SS4");
                tmp_st.push(tmp);
                v = Sbeg;
                break;
            case S18:
                if(c_type == LEX_UNTIL)
                {
                    gl();
                    v = S19;
                }
            case S19:
                st_l.push("(SE5");
                v = Ebeg;
                break;

            case Send:
                if (st_l.top() == "(BS")
                {
                    st_l.pop();
                    v = B2;
                }
                else if (st_l.top() == "(SS1")
                {
                    tmp = tmp_st.top();
                    tmp_st.pop();
                    tmp.pl1 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_GO));
                    prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),tmp.pl0);
                    st_l.pop();
                    v = S6;
                }
                else if (st_l.top() == "(SS2")
                {
                    tmp = tmp_st.top();
                    tmp_st.pop();
                    prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),tmp.pl1);
                    st_l.pop();
                    v = Send;
                }
                else if (st_l.top() == "(SS3")
                {
                    tmp = tmp_st.top();
                    tmp_st.pop();
                    prog.put_lex(Lex(POLIZ_LABEL,tmp.pl0));
                    prog.put_lex(Lex(POLIZ_GO));
                    prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),tmp.pl1);
                    st_l.pop();
                    v = Send;
                }
                else if(st_l.top() == "(SS4")
                {
                    tmp = tmp_st.top();
                    tmp_st.pop();
                    st_l.pop();
                    v = S18;
                }
                else
                    throw "error1";
                break;

            case Xend:
                if (st_l.top() == "(EX1")
                {
                    st_l.pop();
                    v = E1;
                }
                else if (st_l.top() == "(EX2")
                {
                    check_op();
                    st_l.pop();
                    v = Eend;
                }
                else
                    throw "error3";
                break;
            case X1:
                if (c_type == LEX_OR || c_type == LEX_PLUS || c_type == LEX_MINUS)
                {
                    st_l.push("(XT2");
                    st_lex.push(c_type);
                    gl();
                    v = Tbeg;
                }
                else
                    v = Xend;
                break;
            case Ebeg:
                st_l.push("(EX1");
                v = Xbeg;
                break;
            case Tbeg:
                st_l.push("(TF1");
                v = Fbeg;
                break;
            case Tend:
                if (st_l.top() == "(XT1")
                {
                    st_l.pop();
                    v = X1;
                }
                else if (st_l.top() == "(XT2")
                {
                    check_op();
                    st_l.pop();
                    v = X1;
                }
                else
                    throw "error2";
                break;
            case T1:
                if (c_type == LEX_AND || c_type == LEX_TIMES || c_type == LEX_SLASH)
                {
                    st_l.push("(TF2");
                    st_lex.push(c_type);
                    gl();
                    v = Fbeg;
                }
                else
                    v = Tend;
                break;
            case Xbeg:
                st_l.push("(XT1");
                v = Tbeg;
                break;


            case Eend:
                //cerr<<" "<<st_l.top()<<' ';
                if (st_l.top() == "(SE1")
                {
                    eq_type();
                    // cerr<<"after eq_type()"<<endl;
                    prog.put_lex(Lex(LEX_ASSIGN));
                    st_l.pop();
                    v = Send;
                }
                else if (st_l.top() == "(SE2")
                {
                    eq_bool();
                    tmp.pl0 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_FGO));
                    st_l.pop();
                    v = S4;
                }
                else if (st_l.top() == "(SE3")
                {                    
                    eq_bool();
                    tmp.pl1 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_FGO));
                    st_l.pop();
                    v = S9;
                }
                else if (st_l.top() == "(SE4")
                {
                    st_lex.pop();
                    st_l.pop();
                    v = S16;
                }
                else if(st_l.top() == "(SE5")
                {
                    eq_bool();
                    prog.put_lex(Lex(POLIZ_LABEL,tmp.pl0));
                    prog.put_lex(Lex(POLIZ_FGO));
                    st_l.pop();
                    v = Send;
                }
                else if (st_l.top() == "(FE")
                {
                    st_l.pop();
                    v = F2;           
                }
                else 
                    throw "error4";
                break;
            case E1:
                if (c_type == LEX_EQ || c_type == LEX_NEQ || c_type == LEX_LSS || 
                        c_type == LEX_GTR || c_type == LEX_LEQ || c_type == LEX_GEQ)
                {
                    st_lex.push(c_type);
                    gl();
                    v = E2;
                }
                else
                {
                    v = Eend;
                }
                break;
            case E2:
                st_l.push("(EX2");
                v = Xbeg;
                break;
            case Fbeg:
                if (c_type == LEX_LPAREN)
                {
                    gl();
                    v = F1;    
                }
                else if (c_type == LEX_NOT)

                {

                    gl();
                    v = F3;
                }
                else if (c_type == LEX_ID)
                {
                    check_id();
                    prog.put_lex(Lex(curr_lex));
                    gl();
                    v = Fend;
                }
                else if (c_type == LEX_NUM)
                {
                    st_lex.push(LEX_INT);
                    prog.put_lex(Lex(curr_lex));
                    gl();
                    v = Fend;
                }
                else if (c_type == LEX_TRUE || c_type == LEX_FALSE)
                {
                    st_lex.push(LEX_BOOL);
                    prog.put_lex(Lex(curr_lex));
                    gl();
                    v = Fend;
                }
                else 
                    throw "Wrong expression";
                break;
            case Fend:
                if (st_l.top() == "(TF1")
                {
                    st_l.pop();
                    v = T1;
                }
                else if (st_l.top() == "(TF2")
                {
                    check_op();
                    st_l.pop();
                    v = T1;
                }
                else if (st_l.top() == "(FF")
                {
                    check_not();
                    st_l.pop();
                    //   check_not();
                    v = Fend;
                }
                else
                    throw "Expression ERROR";
                break;  
            case F1:
                st_l.push("(FE");
                v = Ebeg;
                break;
            case F2:
                if (c_type == LEX_RPAREN)
                {
                    gl();
                    v = Fend;
                }
                else
                    throw "')' expected";
                break;
            case F3:
                st_l.push("(FF");
                v = Fbeg;
                break;     
        }           
    } while (v != Pend);
    if (st_l.top() != "(st") {
        throw "L_Stack is not empty";
    }
    else if (c_type != LEX_FIN) {
        throw "Unexpected Lexeme";
    }
    else {
        st_l.pop();
    }
    cout << endl << "Yes!!!" << endl;
}

void Parser::dec ( type_of_lex type ) 
{
    int i;
    while ( !st_int.empty()) 
    {
        i = st_int.top();
        st_int.pop();
        if ( TID[i].get_declare() ) 
            throw "twice";
        else 
        {
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}
void Parser::check_id () 
{
    if ( TID[c_val].get_declare() )
        st_lex.push ( TID[c_val].get_type() );
    else 
        throw "not declared";
}
void Parser::check_op () 
{
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
    t2 = st_lex.top(); 
    st_lex.pop();
    op = st_lex.top();
    st_lex.pop();
    t1 = st_lex.top();
    st_lex.pop();
    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
        r = LEX_INT;
    if (op == LEX_OR || op == LEX_AND)
        t = LEX_BOOL;
    if (t1 == t2  &&  t1 == t) 
        st_lex.push(r);
    else
        throw "wrong types are in operation";
    //st_lex.push(r);
    
   // cerr<<"t2 = "<<t2<<' ';
   // cerr<<"t1 = "<<t1<<' ';
    prog.put_lex (Lex (op) );
}
void Parser::check_not () 
{
    type_of_lex temp = st_lex.top();
    st_lex.pop();
    if (temp != LEX_BOOL)
        throw "wrong type is in not";
    else 
    {
        st_lex.push (LEX_BOOL);
        prog.put_lex (Lex (LEX_NOT));
    }
}


void Parser::eq_type () 
{
    type_of_lex temp = st_lex.top();
    st_lex.pop();

    //       cerr<< "eq_type after first pop()  temp= "<<temp<<' ';
    if (temp != st_lex.top())
        throw "wrong types are in :=";
    else
    {
        st_lex.pop();
        //cerr<<"eq_type after last pop() "<<' ';

    }
}
void Parser::eq_bool () 
{

    type_of_lex temp = st_lex.top();
    st_lex.pop();
    if ( temp  != LEX_BOOL )
        throw "expression is not boolean";
}
void Parser::check_id_in_read ()
{
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}


////// MAIN //////
/*
   int main() {
   Scanner scanner("prog.txt");
   int i;
   for (i = 0; i < 3 ; i++)
   cout<<scanner.get_lex()<<endl;
   return 0;
   }
   */

int main ()
{
    try
    {        
       // Parser I ("prw.txt");
      //  I.analyze ();
     //   Interpretator I ("fib.txt");
        Interpretator I ("fac.txt");
        
      // Interpretator I ("seqa.txt");  
       // I.prog.print();
       I.interpretation();
        return 0;
    }
    catch (char c)
    {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch (Lex l)
    {
        cout << "unexpected lexeme";
        cout << l;
        return 1;
    }
    catch ( const char *source )
    {
        cout << source << endl;
        return 1;
    }
}
