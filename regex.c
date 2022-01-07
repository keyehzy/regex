#include "bdd.h"

typedef enum {
  TOK_CHAR,
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

spec("regex") {
  it("should read a single character") {
    tokenizer tokenizer = new_tokenizer("a");
    token t = tokenizer.last_token;
    check(t.type == TOK_CHAR);
  }
}
