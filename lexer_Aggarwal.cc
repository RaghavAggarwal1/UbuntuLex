/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM"// TODO: Add labels for new token types here (as string)
};
// TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    char a,b,d,e,f,g,h,t;
    char table[100];
    //*******
    int flag0= 0;
    int flag8 = 0;
    int fb =0;

    //*****
    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
            flag0 = 1;
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
                if(c == '0'){
                    fb = 0;
                }
                if( c == '9' || c == '8'){
                    flag8 = 1;
                }
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }

        input.GetChar(a);
        if(a == 'x'){
            input.GetChar(b);
            if(b == ' '){
                input.UngetChar(b);
            }
            if(b == '0'){
                input.GetChar(d);
                if(flag8 == 0 && d == '8'){
                    
                    tmp.lexeme = tmp.lexeme+a+b+d;
                    tmp.token_type = BASE08NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else{
                    input.UngetChar(d);
                }
                input.UngetChar(b);
                
            }
            else if (b == '1')
            {
                input.GetChar(d);
                if(d == '6')
                {
                    
                    tmp.lexeme = tmp.lexeme+a+b+d;
                    tmp.token_type = BASE16NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else{
                    input.UngetChar(d);
                }   
                input.UngetChar(b);
            }
            input.UngetChar(a);
        }
        else if (a == '.')
        {
            int num1 = 0, num2 = 0;
            string tem1;
            input.GetChar(b);
            if(isdigit(b)){
                while(isdigit(b) && !input.EndOfInput()){
                    num1 = num1 +1;
                    if(b == '0'){
                        num2 = num2 +1;
                    }
                    tem1 = tem1 + b;
                    input.GetChar(b);
                }
                if (!input.EndOfInput()){
                    input.UngetChar(b);
                }
                if(flag0 == 1 && num1 == num2){
                    input.UngetString(tem1);
                }
                else{
                    input.UngetString(tem1);
                    input.GetChar(b);
                    tmp.lexeme = tmp.lexeme + '.';
                    while ((isdigit(b)) && !input.EndOfInput()){
                        tmp.lexeme = tmp.lexeme + b;
                        input.GetChar(b);
                    }
                    if(!input.EndOfInput()){
                        input.UngetChar(b);
                    }

                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                    return tmp;
                }

            }
            else{
                input.UngetChar(b);
            }
            input.UngetChar(a);
        }
        else if(a == 'F' || a == 'E' || a == 'D' || a == 'C' || a == 'B' || a == 'A'){
            int num1 = 0, num2 =0;

            input.UngetChar(a);
            input.GetChar(table[num1]);

            while((!input.EndOfInput() && table[num1] == 'B' || table[num1] == 'C' || table[num1] == 'A' || table[num1] == 'E' || table[num1] == 'F' || table[num1] == 'D') || isdigit(table[num1])){
                num1 = num1 + 1;
                num2 = num2 + 1;
                input.GetChar(table[num1]);
            }
            if(table[num1] == 'x'){
                input.GetChar(table[num1+1]);
                if(table[num1+1] == '1'){
                    input.GetChar(table[num1+2]);
                    if(table[num1+2] == '6'){
                        while(num1 > -3){
                            input.UngetChar(table[num1+2]);
                            num1 = num1 - 1;
                        }
                        while(num2 > -3){
                            num2 = num2 -1;
                            input.GetChar(c);
                            tmp.lexeme = tmp.lexeme + c;
                        }
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;
                    }
                    else{
                        input.UngetChar(table[num1+2]);
                        input.UngetChar(table[num1+1]);
                    }
                }
                else{
                    input.UngetChar(table[num1+1]);
                }
            }

            while (num1 > 0)
            {
                input.UngetChar(table[num1]);
                num1 = num1 -1;
            }
            input.UngetChar(a);
        }
        else{
            input.UngetChar(a);
        }

        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!

//***************************************************************************************
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
