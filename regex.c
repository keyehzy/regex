#include "bdd.h"

#define ENUMERATE_SPECIAL_CHARACTERS(O) \
  O('.')                                \
  O('?')                                \
  O('*')                                \
  O('+')                                \
  O('|')                                \
  O('(')                                \
  O(')')

#define CASE(e) case e:
#define CASE_SPECIAL_CHARACTERS ENUMERATE_SPECIAL_CHARACTERS(CASE)

typedef enum {
  TOK_CHAR,
  TOK_WILD = '.',
  TOK_QUES = '?',
  TOK_AST = '*',
  TOK_PLUS = '+',
  TOK_OR = '|',
  TOK_LEFT_PAREN = '(',
  TOK_RIGHT_PAREN = ')',
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
} parser;

parser new_parser(const char* str) {
  tokenizer toks = new_tokenizer(str);
  return (parser) {.tok_state = toks};
}

token parser_peek(parser *p) {
  return p->tok_state.last_token;
}

void parser_skip(parser *p) {
  p->tok_state.last_token = tokenizer_next(&p->tok_state);
}

int match(parser *p, const char* to_match) {
  switch(parser_peek(p).type) {
  case TOK_CHAR:
    if(parser_peek(p).begin[0] == to_match[0]) {
      return 1;
    } else {
      return 0;
    }
    break;
  default:
    break;
  }
  return 0;
}

spec("regex") {
  it("tokenizer should read a single character") {
    tokenizer tokenizer = new_tokenizer("a");
    token t = tokenizer.last_token;
    check(t.type == TOK_CHAR);
  }

  it("tokenizer should read a single character for each special character") {

    const int len = 7;
    const char *special_chars[] = {".", "?", "*", "+", "|", "(", ")"};
    const token_type special_chars_types[] = {TOK_WILD, TOK_QUES, TOK_AST, TOK_PLUS, TOK_OR, TOK_LEFT_PAREN, TOK_RIGHT_PAREN};

    for (int i = 0; i < len; i++) {
      tokenizer tokenizer = new_tokenizer(special_chars[i]);
      token t = tokenizer.last_token;
      check(t.type == special_chars_types[i]);
    }
  }

  it("parser should match single character") {
    parser p = new_parser("a");
    check(match(&p, "a") == 1);
  }
}
