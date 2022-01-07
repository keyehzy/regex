#include "bdd.h"

#define ENUMERATE_SPECIAL_CHARACTERS(O)         \
  O('.')                                        \
  O('?')                                        \
  O('*')                                        \
  O('+')                                        \
  O('|')                                        \
  O('(')                                        \
  O(')')                                        \
  O('\0')

#define CASE(e) case e:
#define CASE_SPECIAL_CHARACTERS ENUMERATE_SPECIAL_CHARACTERS(CASE)

typedef enum {
  TOK_CHAR = 'c',
  TOK_WILD = '.',
  TOK_QUES = '?',
  TOK_AST = '*',
  TOK_PLUS = '+',
  TOK_OR = '|',
  TOK_LEFT_PAREN = '(',
  TOK_RIGHT_PAREN = ')',
  TOK_EOF = '\0',
} token_type;

typedef struct {
  token_type type;
  const char* begin;
  unsigned int size;
} token;

typedef struct {
  const char *position;
  const char *original_input;
  token last_token;
} tokenizer;

token tokenizer_next(tokenizer *state) {
  const char *begin = state->position;

  token t = {0};

  switch(begin[0]) {
    CASE_SPECIAL_CHARACTERS
      state->position += 1;
    return (token) {.type = (token_type) begin[0], .begin = begin, .size  = 1};
  default:
    state->position += 1;
    return (token) {.type = TOK_CHAR, .begin = begin, .size  = 1};
  }

  return t;
}

tokenizer new_tokenizer(const char* string) {
  tokenizer t = {0};
  t.position = string;
  t.original_input = string;
  t.last_token = tokenizer_next(&t);
  return t;
}

typedef struct {
  tokenizer tok_state;
  token prev_token;
} parser;

parser new_parser(const char* str) {
  tokenizer toks = new_tokenizer(str);
  return (parser) {.tok_state = toks, .prev_token = toks.last_token};
}

token parser_peek(parser *p) {
  return p->tok_state.last_token;
}

void parser_skip(parser *p) {
  p->prev_token = p->tok_state.last_token;
  p->tok_state.last_token = tokenizer_next(&p->tok_state);
}

int match(parser *p, const char* to_match);

int match_modifier(parser *p, const char* to_match) {

  for(;;) {
    switch(parser_peek(p).type) {
    case TOK_CHAR:
    case TOK_WILD:
    case TOK_EOF:
      return match(p, to_match);
    case TOK_QUES:
      if(p->prev_token.begin[0] == to_match[0])
        to_match += 1;
      parser_skip(p);
      break;
    }
  }
}

int match(parser *p, const char* to_match) {

  for(;;) {
    switch(parser_peek(p).type) {
    case TOK_CHAR: {
      parser_skip(p);
      return match_modifier(p, to_match);
    }
      break;
    case TOK_WILD:
      parser_skip(p);
      to_match += 1;
      break;
    case TOK_EOF:
      return 1;
    case TOK_QUES:              /* ERROR */
    case TOK_OR:
    case TOK_PLUS:
    case TOK_AST:
    default:
      return 0;
    }
  }
}

spec("regex") {
  it("tokenizer should read a single character") {
    tokenizer tokenizer = new_tokenizer("a");
    token t = tokenizer.last_token;
    check(t.type == TOK_CHAR);
  }

  it("tokenizer should read a single character for each special character") {

    const int len = 8;
    const char *special_chars[] = {".", "?", "*", "+", "|", "(", ")", "\0"};
    const token_type special_chars_types[] = {TOK_WILD, TOK_QUES, TOK_AST, TOK_PLUS, TOK_OR, TOK_LEFT_PAREN, TOK_RIGHT_PAREN, TOK_EOF};

    for (int i = 0; i < len; i++) {
      tokenizer tokenizer = new_tokenizer(special_chars[i]);
      token t = tokenizer.last_token;
      check(t.type == special_chars_types[i]);
    }
  }

  it("parser should match single character") {
    parser p = new_parser("a");
    check(match(&p, "a"));
  }

  it("parser should match multiple character") {
    parser p = new_parser("abc");
    check(match(&p, "abc"));
  }

  it("parser should match anything to wildcard character") {
    const int alphabet_len = 26;
    const char* alphabet[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "x", "y", "w", "z"};
    for(int i = 0; i < alphabet_len; i++) {
      parser p = new_parser(".");
      check(match(&p, alphabet[i]));
    }
  }

  it("parser should match question mark (?) with zero or one occurence of preceding element") {
    {
      parser p = new_parser("ab?");
      check(match(&p, "a"));
    }

    {
      parser p = new_parser("ab?");
      check(match(&p, "ab"));
    }

  }

}
