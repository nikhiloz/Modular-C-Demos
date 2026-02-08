/*
 * Chapter 18 — Lexical Analysis (Tokenization)
 *
 * Demonstrates how a compiler breaks source code into tokens:
 *   1. What tokens are: keywords, identifiers, literals, operators, punctuation
 *   2. A hand-written tokenizer for a C subset
 *   3. How real compilers do lexing (DFA, maximal munch)
 *   4. Token categories with examples
 *
 * The mini-tokenizer handles:
 *   Keywords:    int, return
 *   Identifiers: [a-zA-Z_][a-zA-Z0-9_]*
 *   Integers:    [0-9]+
 *   Operators:   + - * / =
 *   Punctuation: ; ( ) { }
 *   Whitespace:  (skipped)
 *   Comments:    // line comments and block comments
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/18_lexical_analysis \
 *            src/18_lexical_analysis/lexical_analysis.c
 * Run:   ./bin/18_lexical_analysis
 *
 * Try:
 *   gcc -E -dD src/18_lexical_analysis/lexical_analysis.c | head -50
 *   gcc -fsyntax-only src/18_lexical_analysis/lexical_analysis.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: What Are Tokens?
 * ════════════════════════════════════════════════════════════════ */

static void demo_token_concepts(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: What Are Tokens?                       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("A token is the smallest meaningful unit the compiler sees.\n");
    printf("The lexer (tokenizer) reads raw characters and groups them.\n\n");

    printf("── Token Categories ──\n\n");
    printf("  ┌──────────────────┬───────────────────────────────────┐\n");
    printf("  │ Category         │ Examples                          │\n");
    printf("  ├──────────────────┼───────────────────────────────────┤\n");
    printf("  │ Keywords         │ int, return, if, while, for       │\n");
    printf("  │ Identifiers      │ main, count, myVar, _flag         │\n");
    printf("  │ Integer literals │ 42, 0xFF, 077, 0b1010             │\n");
    printf("  │ Float literals   │ 3.14, 1e-5, .5f                   │\n");
    printf("  │ String literals  │ \"hello\", \"world\\n\"                │\n");
    printf("  │ Char literals    │ 'a', '\\n', '\\0'                   │\n");
    printf("  │ Operators        │ +  -  *  /  =  ==  !=  &&  ||    │\n");
    printf("  │ Punctuation      │ ;  ,  (  )  {  }  [  ]           │\n");
    printf("  └──────────────────┴───────────────────────────────────┘\n\n");

    printf("── Example: tokenizing  int x = 42;  ──\n\n");
    printf("  Source:  i n t   x   =   4 2 ;\n");
    printf("           ─────  ───  ─  ────  ─\n");
    printf("  Tokens:  [KW:int] [ID:x] [OP:=] [INT:42] [PUNCT:;]\n\n");

    printf("Key rules:\n");
    printf("  • Whitespace separates tokens but is not itself a token\n");
    printf("  • Comments are stripped (treated like whitespace)\n");
    printf("  • Maximal munch: '==' is ONE token, not two '=' tokens\n");
    printf("  • The lexer does NOT understand grammar or meaning\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Mini Tokenizer — Data Structures
 * ════════════════════════════════════════════════════════════════ */

/* Token types for our mini C-subset lexer */
typedef enum {
    TOK_KW_INT,         /* keyword: int     */
    TOK_KW_RETURN,      /* keyword: return  */
    TOK_IDENTIFIER,     /* [a-zA-Z_][a-zA-Z0-9_]* */
    TOK_INT_LITERAL,    /* [0-9]+           */
    TOK_PLUS,           /* +                */
    TOK_MINUS,          /* -                */
    TOK_STAR,           /* *                */
    TOK_SLASH,          /* /                */
    TOK_ASSIGN,         /* =                */
    TOK_SEMICOLON,      /* ;                */
    TOK_LPAREN,         /* (                */
    TOK_RPAREN,         /* )                */
    TOK_LBRACE,         /* {                */
    TOK_RBRACE,         /* }                */
    TOK_EOF,            /* end of input     */
    TOK_ERROR           /* unrecognised     */
} TokenType;

/* Human-readable names */
static const char *token_type_name(TokenType t)
{
    switch (t) {
        case TOK_KW_INT:      return "KW_INT";
        case TOK_KW_RETURN:   return "KW_RETURN";
        case TOK_IDENTIFIER:  return "IDENTIFIER";
        case TOK_INT_LITERAL: return "INT_LITERAL";
        case TOK_PLUS:        return "PLUS";
        case TOK_MINUS:       return "MINUS";
        case TOK_STAR:        return "STAR";
        case TOK_SLASH:       return "SLASH";
        case TOK_ASSIGN:      return "ASSIGN";
        case TOK_SEMICOLON:   return "SEMICOLON";
        case TOK_LPAREN:      return "LPAREN";
        case TOK_RPAREN:      return "RPAREN";
        case TOK_LBRACE:      return "LBRACE";
        case TOK_RBRACE:      return "RBRACE";
        case TOK_EOF:         return "EOF";
        case TOK_ERROR:       return "ERROR";
    }
    return "UNKNOWN";
}

/* A single token */
typedef struct {
    TokenType type;
    char      text[64];     /* the lexeme (actual text) */
    int       line;         /* source line number        */
    int       col;          /* source column number      */
} Token;

#define MAX_TOKENS 256

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Mini Tokenizer — Implementation
 * ════════════════════════════════════════════════════════════════ */

typedef struct {
    const char *src;        /* source string */
    int         pos;        /* current position */
    int         line;       /* current line */
    int         col;        /* current column */
} Lexer;

static void lexer_init(Lexer *lex, const char *source)
{
    lex->src  = source;
    lex->pos  = 0;
    lex->line = 1;
    lex->col  = 1;
}

static char lexer_peek(Lexer *lex)
{
    return lex->src[lex->pos];
}

static char lexer_advance(Lexer *lex)
{
    char c = lex->src[lex->pos++];
    if (c == '\n') {
        lex->line++;
        lex->col = 1;
    } else {
        lex->col++;
    }
    return c;
}

static int lexer_at_end(Lexer *lex)
{
    return lex->src[lex->pos] == '\0';
}

/* Skip whitespace and comments */
static void lexer_skip_whitespace_comments(Lexer *lex)
{
    while (!lexer_at_end(lex)) {
        char c = lexer_peek(lex);

        /* Whitespace */
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            lexer_advance(lex);
            continue;
        }

        /* Line comment: // ... */
        if (c == '/' && lex->src[lex->pos + 1] == '/') {
            lexer_advance(lex); /* skip / */
            lexer_advance(lex); /* skip / */
            while (!lexer_at_end(lex) && lexer_peek(lex) != '\n') {
                lexer_advance(lex);
            }
            continue;
        }

        /* Block comment: slash-star ... star-slash */
        if (c == '/' && lex->src[lex->pos + 1] == '*') {
            lexer_advance(lex); /* skip / */
            lexer_advance(lex); /* skip * */
            while (!lexer_at_end(lex)) {
                if (lexer_peek(lex) == '*' && lex->src[lex->pos + 1] == '/') {
                    lexer_advance(lex); /* skip * */
                    lexer_advance(lex); /* skip / */
                    break;
                }
                lexer_advance(lex);
            }
            continue;
        }

        break; /* Not whitespace or comment */
    }
}

/* Read the next token */
static Token lexer_next_token(Lexer *lex)
{
    Token tok;
    memset(&tok, 0, sizeof(tok));

    lexer_skip_whitespace_comments(lex);

    tok.line = lex->line;
    tok.col  = lex->col;

    if (lexer_at_end(lex)) {
        tok.type = TOK_EOF;
        strcpy(tok.text, "<EOF>");
        return tok;
    }

    char c = lexer_peek(lex);

    /* Single-character tokens */
    switch (c) {
        case '+': tok.type = TOK_PLUS;      break;
        case '-': tok.type = TOK_MINUS;     break;
        case '*': tok.type = TOK_STAR;      break;
        case '=': tok.type = TOK_ASSIGN;    break;
        case ';': tok.type = TOK_SEMICOLON; break;
        case '(': tok.type = TOK_LPAREN;    break;
        case ')': tok.type = TOK_RPAREN;    break;
        case '{': tok.type = TOK_LBRACE;    break;
        case '}': tok.type = TOK_RBRACE;    break;
        default:  tok.type = TOK_ERROR;     break;
    }

    if (tok.type != TOK_ERROR) {
        tok.text[0] = c;
        tok.text[1] = '\0';
        lexer_advance(lex);
        return tok;
    }

    /* Integer literal: [0-9]+ */
    if (isdigit((unsigned char)c)) {
        int i = 0;
        while (!lexer_at_end(lex) && isdigit((unsigned char)lexer_peek(lex))) {
            if (i < 62) tok.text[i++] = lexer_peek(lex);
            lexer_advance(lex);
        }
        tok.text[i] = '\0';
        tok.type = TOK_INT_LITERAL;
        return tok;
    }

    /* Identifier or keyword: [a-zA-Z_][a-zA-Z0-9_]* */
    if (isalpha((unsigned char)c) || c == '_') {
        int i = 0;
        while (!lexer_at_end(lex) &&
               (isalnum((unsigned char)lexer_peek(lex)) || lexer_peek(lex) == '_')) {
            if (i < 62) tok.text[i++] = lexer_peek(lex);
            lexer_advance(lex);
        }
        tok.text[i] = '\0';

        /* Check for keywords */
        if (strcmp(tok.text, "int") == 0)    tok.type = TOK_KW_INT;
        else if (strcmp(tok.text, "return") == 0) tok.type = TOK_KW_RETURN;
        else tok.type = TOK_IDENTIFIER;
        return tok;
    }

    /* Unrecognised character */
    tok.type = TOK_ERROR;
    tok.text[0] = c;
    tok.text[1] = '\0';
    lexer_advance(lex);
    return tok;
}

/* Tokenize an entire source string and return count */
static int tokenize_all(const char *source, Token *tokens, int max_tokens)
{
    Lexer lex;
    lexer_init(&lex, source);

    int count = 0;
    while (count < max_tokens) {
        tokens[count] = lexer_next_token(&lex);
        if (tokens[count].type == TOK_EOF) {
            count++;
            break;
        }
        count++;
    }
    return count;
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Tokenizer Demo
 * ════════════════════════════════════════════════════════════════ */

static void print_tokens(Token *tokens, int count)
{
    printf("  ┌─────┬───────────────┬────────────────────┬──────┐\n");
    printf("  │  #  │ Type          │ Lexeme             │ Loc  │\n");
    printf("  ├─────┼───────────────┼────────────────────┼──────┤\n");

    for (int i = 0; i < count; i++) {
        printf("  │ %3d │ %-13s │ %-18s │ %d:%-2d │\n",
               i + 1,
               token_type_name(tokens[i].type),
               tokens[i].text,
               tokens[i].line,
               tokens[i].col);
    }

    printf("  └─────┴───────────────┴────────────────────┴──────┘\n\n");
}

static void demo_tokenizer(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2-4: Mini Tokenizer Demo                  ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* ── Test 1: Simple variable declaration ── */
    const char *src1 = "int x = 42;";
    printf("── Test 1: \"%s\" ──\n\n", src1);

    Token tokens1[MAX_TOKENS];
    int count1 = tokenize_all(src1, tokens1, MAX_TOKENS);
    print_tokens(tokens1, count1);

    /* ── Test 2: A small function ── */
    const char *src2 =
        "int add(int a, int b) {\n"
        "    return a + b;\n"
        "}";
    printf("── Test 2: A small function ──\n");
    printf("  Source:\n    %s\n\n", "int add(int a, int b) { return a + b; }");

    Token tokens2[MAX_TOKENS];
    int count2 = tokenize_all(src2, tokens2, MAX_TOKENS);
    print_tokens(tokens2, count2);

    /* ── Test 3: With comments ── */
    const char *src3 =
        "// compute sum\n"
        "int sum = a + b; /* result */";
    printf("── Test 3: Source with comments ──\n");
    printf("  Source:\n    // compute sum\\n"
           "    int sum = a + b; /* result */\n\n");

    Token tokens3[MAX_TOKENS];
    int count3 = tokenize_all(src3, tokens3, MAX_TOKENS);
    print_tokens(tokens3, count3);

    printf("  Notice: Comments are stripped — the parser never sees them.\n\n");

    /* ── Test 4: Expression ── */
    const char *src4 = "result = 3 + 4 * 2 - 1;";
    printf("── Test 4: \"%s\" ──\n\n", src4);

    Token tokens4[MAX_TOKENS];
    int count4 = tokenize_all(src4, tokens4, MAX_TOKENS);
    print_tokens(tokens4, count4);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: How the Real Compiler Lexer Works
 * ════════════════════════════════════════════════════════════════ */

static void demo_real_lexer(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: How the Real Compiler Lexer Works      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("Our mini tokenizer uses if-else chains. Real compilers use:\n\n");

    printf("── Finite Automata (DFA/NFA) ──\n\n");
    printf("  A DFA (Deterministic Finite Automaton) is a state machine:\n");
    printf("    • States: START, IN_NUMBER, IN_IDENTIFIER, IN_STRING, ...\n");
    printf("    • Transitions: on each character, move to next state\n");
    printf("    • Accept states: emit a token when reached\n\n");

    printf("  Example DFA for integer literals:\n");
    printf("    START ──[0-9]──→ IN_NUMBER ──[0-9]──→ IN_NUMBER\n");
    printf("                                  │\n");
    printf("                          [^0-9]──→ ACCEPT (emit INT_LITERAL)\n\n");

    printf("── Maximal Munch Rule ──\n\n");
    printf("  The lexer always takes the LONGEST possible match:\n");
    printf("    '=='  → one EQ_EQ token    (not two ASSIGN tokens)\n");
    printf("    '++a' → one INCREMENT + ID  (not two PLUS + ID)\n");
    printf("    '->x' → one ARROW + ID      (not MINUS > ID)\n\n");

    printf("  This is why:  a+++b   is parsed as  (a++) + (b)\n");
    printf("  and NOT as:   a + (++b)\n\n");

    printf("── Lexer generators ──\n\n");
    printf("  • flex (lex)  — generates C lexer from regex rules\n");
    printf("  • re2c        — fast lexer generator based on DFAs\n");
    printf("  • Hand-written (like GCC/Clang) — more control\n\n");

    printf("── GCC's actual lexer ──\n\n");
    printf("  • Written in C, hand-coded (~5000 lines in libcpp)\n");
    printf("  • Handles trigraphs, digraphs, UCNs, string concat\n");
    printf("  • Integrated with the preprocessor\n");
    printf("  • Produces cpp_token structures\n\n");

    printf("  Try: gcc -E -dD %s | head -30\n", __FILE__);
    printf("       (see how the preprocessor outputs tokens)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Token Categories Deep Dive
 * ════════════════════════════════════════════════════════════════ */

static void demo_token_categories(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Token Categories Deep Dive             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Keywords (C99 has 37) ──\n");
    printf("  auto     break    case     char     const\n");
    printf("  continue default  do       double   else\n");
    printf("  enum     extern   float    for      goto\n");
    printf("  if       inline   int      long     register\n");
    printf("  restrict return   short    signed   sizeof\n");
    printf("  static   struct   switch   typedef  union\n");
    printf("  unsigned void     volatile while    _Bool\n");
    printf("  _Complex _Imaginary\n\n");

    printf("── Identifier rules ──\n");
    printf("  • Must start with [a-zA-Z_]\n");
    printf("  • Followed by [a-zA-Z0-9_]*\n");
    printf("  • Case-sensitive: 'count' ≠ 'Count'\n");
    printf("  • Cannot be a keyword\n");
    printf("  • Names starting with _ are reserved in some contexts\n\n");

    printf("── Integer literal forms ──\n");
    printf("  Decimal:     42        1000\n");
    printf("  Octal:       052       (leading 0)\n");
    printf("  Hexadecimal: 0x2A      (leading 0x)\n");
    printf("  Binary:      0b101010  (C23 / GCC extension)\n");
    printf("  Suffixes:    42L  42U  42ULL  42LL\n\n");

    printf("── Operators (multi-character) ──\n");
    printf("  These require maximal-munch:\n");
    printf("  ==  !=  <=  >=  &&  ||  <<  >>  ++  --\n");
    printf("  +=  -=  *=  /=  %%=  <<=  >>=  &=  |=  ^=\n");
    printf("  ->  ...  (three dots = ellipsis)\n\n");

    printf("── Punctuation vs Operators ──\n");
    printf("  Some symbols serve double duty:\n");
    printf("  * → multiply OR dereference (context-dependent)\n");
    printf("  & → bitwise AND OR address-of\n");
    printf("  - → subtract OR unary negate\n");
    printf("  The LEXER doesn't distinguish — that's the parser's job.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main
 * ════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 18 — Lexical Analysis (Tokenization)      ║\n");
    printf("║  Breaking source code into its atoms               ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    demo_token_concepts();
    demo_tokenizer();
    demo_real_lexer();
    demo_token_categories();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: Characters → Tokens → ready for the Parser\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("  The lexer is the first real stage of compilation.\n");
    printf("  It converts a stream of characters into a stream of\n");
    printf("  tokens that the parser can work with.\n\n");
    printf("  Next up: Chapter 19 — Parsing & AST construction.\n\n");

    return 0;
}
