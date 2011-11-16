#ifndef LEXER_H
#define LEXER_H
typedef enum {T_HEX, T_OCT, T_DEC, T_FLOAT, T_VAR, T_FUN, T_OP, 
              T_LPAREN, T_RPAREN, T_ERROR, T_EOF, T_SPACE} t_type;

typedef struct {
    t_type type;
    int pos, len;
    char * expr;
} token;

token next_tok(char * expr, int i);
#endif
