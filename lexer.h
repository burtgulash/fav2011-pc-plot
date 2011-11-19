#ifndef LEXER_H
#define LEXER_H

enum {T_HEX, T_OCT, T_DEC, T_FLOAT, T_VAR, T_FUN, T_OP, 
      T_LPAREN, T_RPAREN, T_COLON, T_ERROR, T_EOF, T_SPACE};

typedef struct {
    int type;
    int pos, len;
    char * context;
} token;

token * next_tok(char * expr, int i);
#endif
