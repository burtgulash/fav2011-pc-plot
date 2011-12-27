#ifndef LEXER_H
#define LEXER_H

/* all possible token types */
enum {T_HEX, T_OCT, T_DEC, T_FLOAT, T_VAR, T_FUN, T_OP, 
      T_LPAREN, T_RPAREN, T_COLON, T_ERROR, T_EOF, T_SPACE};

/* token structure */
typedef struct {
	/* token type */
    int type; 
	/* position in 'context' string and length of token */
    int pos, len;
    char * context;
} token;

token * next_tok(char * expr, int i);
#endif
