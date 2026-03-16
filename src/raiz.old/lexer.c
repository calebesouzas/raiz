#include <ctype.h> // isdigit(), isalpha(), isalnum()

#include "lexer.h"   // lexer structures declarations
#include "arrays.h"  // array_len(), array_push()
#include "strings.h" // raiz_string_push(), raiz_string_push_slice()

#define raiz_lexer_init()\
  (Raiz_LexerState) {\
    .column = 1,\
    .line = 1,\
    .current = 0,\
    .start = 0,\
    .start_line = 1,\
    .start_column = 1,\
    .tokens = NULL,\
  }

#define array_push(a, v) raiz_array_push((a), (v))
#define array_len(a) raiz_array_len(a)
#define array_free(a) raiz_array_free(a)

#define backup_start(lexer)\
  do {\
    (lexer)->start = (lexer)->current;\
    (lexer)->start_line = (lexer)->line;\
    (lexer)->start_column = (lexer)->column;\
  } while (0)

#define set_token_data(lexer, field, value)\
  (lexer)->tokens[array_len((lexer)->tokens) - 1].data.field = (value)

#define is_identstart(c) (isalpha(c) || (c) == '_' || (c) == '\\')
#define is_identchar(c) (isalnum(c) || (c) == '_')

static void push_token(Raiz_LexerState *state, Raiz_TokenKind kind) {
  Raiz_Token token = {
    .column = state->start_column,
    .line = state->start_line,
    .start = state->start,
    .kind = kind,
    .len = state->current - state->start,
    // '.data' remains unset
  };

  array_push(state->tokens, token);
}

static uint32_t update_state(Raiz_LexerState *state, char current_char) {
  switch (current_char) {
  case '\n':
    state->line++;
    state->column = 1;
    break;
  default:
    state->column++;
    break;
  }

  return state->current++; // increment and returns it self - 1
}

// NOTE: could implement a hash table instead of iterating through
// each keyword and comparing it with 'word'. Or a switch case at word's
// length and iterate over the keywords with the same length only
static void set_if_is_keyword(Raiz_LexerState *state, char*const word) {
  // i know there is a better way, but i don't know this way (not yet)
  unsigned int keywords_start = (unsigned int) RAIZ_TOKEN_LITERAL_STRING + 1;

  size_t len = state->current - state->start;
  const char* raiz_keywords[] = {
    "as", "do", "or",
    "for", "fun", "let", "not", "val", "var",
    "else",
    "match", "shell", "while"
  };

  for (unsigned int i = 0;
      i < (sizeof(raiz_keywords) / sizeof(raiz_keywords[0]));
      ++i) {
    if (strncmp(word, raiz_keywords[i], len) == 0) {
      push_token(state, i + keywords_start); // hope it does work...
      return;
    }
  }
  push_token(state, RAIZ_TOKEN_IDENTIFIER);
  char* identifier = NULL;
  raiz_string_push_slice(identifier, word + state->start, len);
  set_token_data(state, s_val, identifier);
}

static void handle_string_literal(Raiz_LexerState *state, char*const source_code) {
  backup_start(state);
  update_state(state, '"'); // consume '"'

  for (; source_code[state->current] && source_code[state->current] != '"';
       update_state(state, source_code[state->current]));
 
  push_token(state, RAIZ_TOKEN_LITERAL_STRING);

  // with 'i - 1' we ignore the closing '"'
  char* string = NULL;
  raiz_string_push_slice(string, source_code + state->start + 1, 
                    state->current - state->start - 1);
  set_token_data(state, s_val, string);
}

static bool handle_simple_token(Raiz_LexerState* state, char*const source_code) {
  backup_start(state);

  switch (source_code[state->current]) {
  // single character tokens
  // TODO: add TYPE_IDENTIFIER for processing after finding '@' (?)
  // I could use '\' to escape variables with the same name as keywords or any
  // builtin function (basically what the keywords are, bruh)... 
  case '@': push_token(state, RAIZ_TOKEN_AT);       break;
  case '#': push_token(state, RAIZ_TOKEN_HASH);     break;
  case '$': push_token(state, RAIZ_TOKEN_DOLLAR);   break;
  case '%': push_token(state, RAIZ_TOKEN_PERCENT);  break;
  case '+': push_token(state, RAIZ_TOKEN_PLUS);     break;
  case '*': push_token(state, RAIZ_TOKEN_STAR);     break;
  case '/': push_token(state, RAIZ_TOKEN_SLASH);    break;
  case '?': push_token(state, RAIZ_TOKEN_WHAT);     break;
  case '.': push_token(state, RAIZ_TOKEN_DOT);      break;
  case ',': push_token(state, RAIZ_TOKEN_COMMA);    break;
  case ':': push_token(state, RAIZ_TOKEN_COLLON);   break;
  //// paired singlecharacter tokens
  case '(': push_token(state, RAIZ_TOKEN_OPEN_PARENTHESES);     break;
  case ')': push_token(state, RAIZ_TOKEN_CLOSE_PARENTHESES);    break;
  case '[': push_token(state, RAIZ_TOKEN_OPEN_BRACKETS);        break;
  case ']': push_token(state, RAIZ_TOKEN_CLOSE_BRACKETS);       break;
  case '{': push_token(state, RAIZ_TOKEN_OPEN_CURLY_BRACES);    break;
  case '}': push_token(state, RAIZ_TOKEN_CLOSE_CURLY_BRACES);   break;
  // two character tokens
  //// (maybe) composed token
  case '!':
    if (source_code[state->current] == '=') {
      push_token(state, RAIZ_TOKEN_NOT_EQUAL);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_BANG);
    }
    break;
  //// duplicated character tokens
  case '&':
    if (source_code[state->current + 1] == '&') {
      push_token(state, RAIZ_TOKEN_DOUBLE_AMPERSAND);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_AMPERSAND);
    }
    break;
  case '|':
    if (source_code[state->current + 1] == '|') {
      push_token(state, RAIZ_TOKEN_DOUBLE_PIPE);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_PIPE);
    }
    break;
  case '^':
    push_token(state, RAIZ_TOKEN_HAT);
    break;
  //// maybe duplicated or composed two character tokens
  case '=':
    switch (source_code[state->current + 1]) {
    case '=':
      push_token(state, RAIZ_TOKEN_EQUAL);
      state->current++;
      break;
    case '>':
      push_token(state, RAIZ_TOKEN_FAT_ARROW);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_ASSIGN); break;
    }
    break; // case '='
  case '-':
    push_token(state, RAIZ_TOKEN_MINUS);
    break;
  case '>':
    switch (source_code[state->current + 1]) {
    case '>':
      push_token(state, RAIZ_TOKEN_DOUBLE_GREATER);
      state->current++;
      break;
    case '=':
      push_token(state, RAIZ_TOKEN_GREATER_OR_EQUAL);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_GREATER_THAN); break;
    }
    break; // case '>'
  case '<':
    switch (source_code[state->current + 1]) {
    case '<':
      push_token(state, RAIZ_TOKEN_DOUBLE_LESS);
      state->current++;
      break;
    case '=':
      push_token(state, RAIZ_TOKEN_LESS_OR_EQUAL);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_LESS_THAN); break;
    } // switch next char
    break; // case '<'
  default: // switch current char
    return false;
  }

  unsigned int len = state->current - state->start + 1;
  state->tokens[array_len(state->tokens) - 1].len = len;

  return true;
}

#ifdef RAIZ_DEBUG
void raiz_debug_added_token(Raiz_LexerState* state);
#endif

Raiz_Token* raiz_tokenize(char* const source_code) {
  Raiz_LexerState state;
  for (state = raiz_lexer_init();
      source_code[state.current];
      update_state(&state, source_code[state.current]))
  {
    #ifdef RAIZ_DEBUG
    short add = 1;
    #endif

    switch (source_code[state.current]) {
    case '\n':
    case ' ':
    case '\t':
    case '\r':
      #ifdef RAIZ_DEBUG
      add = 0;
      #endif
      break;
    case '"':
      handle_string_literal(&state, source_code);

      if (source_code[state.current] != '"') {
        fprintf(stderr, "raiz: expected '\"' closing string at %u:%u\n",
                state.line, state.column);
        array_free(state.tokens);
        return NULL;
      }
      break;
    default:
      if (handle_simple_token(&state, source_code)) {}
      // NOTE: only 32 bit integer numbers are handled
      // and there are no prefixes or suffixes
      // (such as '0x', '0b' prefixes or 'u' and 'f' suffixes)
      else if (isdigit(source_code[state.current])) {
        // TODO: handle floating numbers
        int number = 0;
        backup_start(&state);

        // if entered this 'if' block, will run the loop below at least once
        // Then, the number and it's length are calculated correctly
        for (; isdigit(source_code[state.current]);
            update_state(&state, source_code[state.current])) {
          // TODO: check for and skip '_'s for better reading of the number
          number = (number * 10) + (source_code[state.current] - '0');
        }

        push_token(&state, RAIZ_TOKEN_LITERAL_INT);
        
        set_token_data(&state, i_val, number);

        // don't skip chars after numbers!
        state.current--;
        state.column--;
      } else if (is_identstart(source_code[state.current])) {
        backup_start(&state);

        // get identifier length
        for (; is_identchar(source_code[state.current]);
            update_state(&state, source_code[state.current]));

        if (source_code[state.start] == '\\') {
          update_state(&state, '\\');

          push_token(&state, RAIZ_TOKEN_IDENTIFIER);
          char* identifier = NULL;
          raiz_string_push_slice(identifier,
                                 source_code + state.start,
                                 state.current - state.start);
          set_token_data(&state, s_val, identifier);
        } else set_if_is_keyword(&state, source_code + state.start);

        state.current--;
        state.column--;
      } else {
        fprintf(
          stderr,
          "raiz: invalid character at %u:%u: '%c'\n",
          state.line,
          state.column + 1,
          source_code[state.current]
        );
        array_free(state.tokens);
        return NULL;
      }
      break; // default case
    } // switch (source_code[i]) // current character

    #ifdef RAIZ_DEBUG
    if (add) raiz_debug_added_token(&state);
    #endif // ifdef RAIZ_DEBUG
  } // for (... source_code ...) // main loop

  return state.tokens;
}

void raiz_debug_added_token(Raiz_LexerState* state) {
  const char* token_variant_names[] = {
    #define RAIZ_DEFINE_TOKEN(variant, string) #variant,
    #include "tokens.def"
    #undef RAIZ_DEFINE_TOKEN
    NULL
  };
  Raiz_Token token = state->tokens[array_len(state->tokens) - 1];
  printf("[RAIZ]: add token [%u:%u:%u] (%s)",
      token.line, token.column, token.len,
      token_variant_names[token.kind]);

  switch (token.kind) {
  case RAIZ_TOKEN_LITERAL_CHAR:
    printf(" < '%c' >\n", token.data.c_val);
    break;
  case RAIZ_TOKEN_LITERAL_INT:
    printf(" < %d >\n", token.data.i_val);
    break;
  case RAIZ_TOKEN_LITERAL_STRING:
    printf(" < \"%s\" >\n", token.data.s_val);
    break;
  case RAIZ_TOKEN_IDENTIFIER:
    printf(" < %s >\n", token.data.s_val);
    break;
  default:
    printf("\n");
    break;
  }
}

// undefine used macros so they don't annoy users later
#undef raiz_lexer_init
#undef backup_start
#undef set_token_data

#undef array_push
#undef array_len
#undef array_free
