/*
 * Chapter 19 — Syntax Analysis (Parsing & AST)
 *
 * Demonstrates how a compiler parses tokens into an Abstract Syntax Tree:
 *   1. Grammar rules (BNF) for arithmetic expressions
 *   2. Recursive descent parser with correct operator precedence
 *   3. AST construction, printing, and evaluation
 *   4. Top-down vs bottom-up parsing concepts
 *
 * The parser handles: integer literals, +, -, *, /, and parentheses.
 *   Precedence: (parentheses) > {*, /} > {+, -}
 *
 * Grammar (BNF):
 *   expr   → term (('+' | '-') term)*
 *   term   → factor (('*' | '/') factor)*
 *   factor → INTEGER | '(' expr ')' | '-' factor
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/19_parsing_ast \
 *            src/19_parsing_ast/parsing_ast.c
 * Run:   ./bin/19_parsing_ast
 *
 * Try:
 *   gcc -fdump-tree-original src/19_parsing_ast/parsing_ast.c
 *   (see GCC's internal tree representation)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Grammar Rules (BNF) Explanation
 * ════════════════════════════════════════════════════════════════ */

static void demo_grammar_concepts(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Grammar Rules (BNF)                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("A grammar defines the STRUCTURE of valid programs.\n");
    printf("We use BNF (Backus-Naur Form) to express it.\n\n");

    printf("── Grammar for arithmetic expressions ──\n\n");
    printf("  expr   → term (('+' | '-') term)*\n");
    printf("  term   → factor (('*' | '/') factor)*\n");
    printf("  factor → INTEGER | '(' expr ')' | '-' factor\n\n");

    printf("── Why this structure? ──\n\n");
    printf("  • 'expr' handles + and - (lowest precedence)\n");
    printf("  • 'term' handles * and / (higher precedence)\n");
    printf("  • 'factor' handles atoms and grouping (highest)\n\n");
    printf("  This naturally encodes operator precedence!\n");
    printf("  Example: 3 + 4 * 2\n");
    printf("    expr sees:  term(3) + term(4*2)\n");
    printf("    NOT:        expr(3+4) * factor(2)\n\n");

    printf("── Parsing approaches ──\n\n");
    printf("  Top-down (we use this!):\n");
    printf("    • Start from the start symbol (expr)\n");
    printf("    • Expand rules, matching input left-to-right\n");
    printf("    • Recursive descent: one function per grammar rule\n");
    printf("    • LL(1) parsers, hand-written or generated\n\n");

    printf("  Bottom-up:\n");
    printf("    • Start from input tokens\n");
    printf("    • Reduce sequences into grammar rules\n");
    printf("    • Shift-reduce, LR(1), LALR(1) parsers\n");
    printf("    • Used by yacc/bison, more powerful but complex\n\n");

    printf("  GCC uses a hand-written recursive descent parser.\n");
    printf("  Clang also uses recursive descent.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Lexer for the Expression Parser
 * ════════════════════════════════════════════════════════════════ */

typedef enum {
    T_INT,      /* integer literal          */
    T_PLUS,     /* +                        */
    T_MINUS,    /* -                        */
    T_STAR,     /* *                        */
    T_SLASH,    /* /                        */
    T_LPAREN,   /* (                        */
    T_RPAREN,   /* )                        */
    T_EOF,      /* end of input             */
    T_ERROR     /* unexpected character     */
} TokType;

typedef struct {
    TokType type;
    int     value;  /* only valid for T_INT */
    char    ch;     /* the character for operators */
} Tok;

typedef struct {
    const char *src;
    int         pos;
} ExprLexer;

static void expr_lexer_init(ExprLexer *lex, const char *source)
{
    lex->src = source;
    lex->pos = 0;
}

static Tok expr_next_token(ExprLexer *lex)
{
    Tok tok = { T_EOF, 0, '\0' };

    /* Skip whitespace */
    while (lex->src[lex->pos] == ' ' || lex->src[lex->pos] == '\t')
        lex->pos++;

    char c = lex->src[lex->pos];
    if (c == '\0') return tok;

    /* Integer literal */
    if (isdigit((unsigned char)c)) {
        tok.type = T_INT;
        tok.value = 0;
        while (isdigit((unsigned char)lex->src[lex->pos])) {
            tok.value = tok.value * 10 + (lex->src[lex->pos] - '0');
            lex->pos++;
        }
        return tok;
    }

    tok.ch = c;
    lex->pos++;

    switch (c) {
        case '+': tok.type = T_PLUS;   return tok;
        case '-': tok.type = T_MINUS;  return tok;
        case '*': tok.type = T_STAR;   return tok;
        case '/': tok.type = T_SLASH;  return tok;
        case '(': tok.type = T_LPAREN; return tok;
        case ')': tok.type = T_RPAREN; return tok;
        default:
            tok.type = T_ERROR;
            return tok;
    }
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: AST Node Definition
 * ════════════════════════════════════════════════════════════════ */

typedef enum {
    NODE_INT,       /* leaf: integer literal     */
    NODE_BINOP,     /* binary operator: +,-,*,/  */
    NODE_UNARY_NEG  /* unary negation: -expr     */
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int      int_value;     /* for NODE_INT              */
    char     op;            /* for NODE_BINOP: '+','-','*','/' */
    struct ASTNode *left;   /* left child (or child for unary) */
    struct ASTNode *right;  /* right child               */
} ASTNode;

static ASTNode *make_int_node(int value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type      = NODE_INT;
    node->int_value = value;
    node->op        = '\0';
    node->left      = NULL;
    node->right     = NULL;
    return node;
}

static ASTNode *make_binop_node(char op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type      = NODE_BINOP;
    node->int_value = 0;
    node->op        = op;
    node->left      = left;
    node->right     = right;
    return node;
}

static ASTNode *make_unary_neg_node(ASTNode *child)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type      = NODE_UNARY_NEG;
    node->int_value = 0;
    node->op        = '-';
    node->left      = child;
    node->right     = NULL;
    return node;
}

static void free_ast(ASTNode *node)
{
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Recursive Descent Parser
 * ════════════════════════════════════════════════════════════════ */

/*
 * Parser state: holds the lexer and the current (lookahead) token.
 * We use LL(1) — one token of lookahead.
 */
typedef struct {
    ExprLexer lexer;
    Tok       current;
} Parser;

static void parser_init(Parser *p, const char *source)
{
    expr_lexer_init(&p->lexer, source);
    p->current = expr_next_token(&p->lexer);
}

static void parser_advance(Parser *p)
{
    p->current = expr_next_token(&p->lexer);
}

static int parser_expect(Parser *p, TokType type)
{
    if (p->current.type == type) {
        parser_advance(p);
        return 1;
    }
    printf("  [PARSE ERROR] Expected token type %d, got %d\n", type, p->current.type);
    return 0;
}

/* Forward declarations for mutual recursion */
static ASTNode *parse_expr(Parser *p);
static ASTNode *parse_term(Parser *p);
static ASTNode *parse_factor(Parser *p);

/*
 * factor → INTEGER | '(' expr ')' | '-' factor
 */
static ASTNode *parse_factor(Parser *p)
{
    /* Unary minus */
    if (p->current.type == T_MINUS) {
        parser_advance(p);
        ASTNode *child = parse_factor(p);
        return make_unary_neg_node(child);
    }

    /* Parenthesised expression */
    if (p->current.type == T_LPAREN) {
        parser_advance(p);  /* consume '(' */
        ASTNode *node = parse_expr(p);
        parser_expect(p, T_RPAREN);  /* consume ')' */
        return node;
    }

    /* Integer literal */
    if (p->current.type == T_INT) {
        int val = p->current.value;
        parser_advance(p);
        return make_int_node(val);
    }

    printf("  [PARSE ERROR] Unexpected token (type=%d)\n", p->current.type);
    return make_int_node(0);
}

/*
 * term → factor (('*' | '/') factor)*
 */
static ASTNode *parse_term(Parser *p)
{
    ASTNode *left = parse_factor(p);

    while (p->current.type == T_STAR || p->current.type == T_SLASH) {
        char op = (p->current.type == T_STAR) ? '*' : '/';
        parser_advance(p);
        ASTNode *right = parse_factor(p);
        left = make_binop_node(op, left, right);
    }

    return left;
}

/*
 * expr → term (('+' | '-') term)*
 */
static ASTNode *parse_expr(Parser *p)
{
    ASTNode *left = parse_term(p);

    while (p->current.type == T_PLUS || p->current.type == T_MINUS) {
        char op = (p->current.type == T_PLUS) ? '+' : '-';
        parser_advance(p);
        ASTNode *right = parse_term(p);
        left = make_binop_node(op, left, right);
    }

    return left;
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: AST Printer (indented tree view)
 * ════════════════════════════════════════════════════════════════ */

static void print_ast_indent(ASTNode *node, int depth, const char *prefix)
{
    if (!node) return;

    /* Print indentation */
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s", prefix);

    switch (node->type) {
        case NODE_INT:
            printf("INT(%d)\n", node->int_value);
            break;
        case NODE_BINOP:
            printf("BINOP '%c'\n", node->op);
            print_ast_indent(node->left,  depth + 1, "├─L: ");
            print_ast_indent(node->right, depth + 1, "└─R: ");
            break;
        case NODE_UNARY_NEG:
            printf("NEG\n");
            print_ast_indent(node->left, depth + 1, "└─ ");
            break;
    }
}

static void print_ast(ASTNode *node)
{
    print_ast_indent(node, 2, "");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: AST Evaluator
 * ════════════════════════════════════════════════════════════════ */

static int eval_ast(ASTNode *node)
{
    if (!node) return 0;

    switch (node->type) {
        case NODE_INT:
            return node->int_value;

        case NODE_BINOP: {
            int left  = eval_ast(node->left);
            int right = eval_ast(node->right);
            switch (node->op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0) {
                        printf("  [RUNTIME ERROR] Division by zero!\n");
                        return 0;
                    }
                    return left / right;
            }
            return 0;
        }

        case NODE_UNARY_NEG:
            return -eval_ast(node->left);
    }

    return 0;
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Parsing Demos
 * ════════════════════════════════════════════════════════════════ */

static void demo_parse_and_eval(const char *expression)
{
    printf("  Expression: \"%s\"\n\n", expression);

    Parser p;
    parser_init(&p, expression);
    ASTNode *ast = parse_expr(&p);

    printf("  AST:\n");
    print_ast(ast);
    printf("\n");

    int result = eval_ast(ast);
    printf("  Result: %d\n\n", result);

    free_ast(ast);
}

static void demo_parsing(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2-7: Recursive Descent Parser Demo        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("════ Demo 1: Simple addition ════\n");
    demo_parse_and_eval("3 + 4");

    printf("════ Demo 2: Precedence — multiplication binds tighter ════\n");
    demo_parse_and_eval("3 + 4 * 2");

    printf("════ Demo 3: Parentheses override precedence ════\n");
    demo_parse_and_eval("(3 + 4) * 2");

    printf("════ Demo 4: The original challenge — 3 + 4 * (2 - 1) ════\n");
    demo_parse_and_eval("3 + 4 * (2 - 1)");

    printf("════ Demo 5: Multiple operations ════\n");
    demo_parse_and_eval("10 - 2 * 3 + 8 / 4");

    printf("════ Demo 6: Nested parentheses ════\n");
    demo_parse_and_eval("((2 + 3) * (4 - 1))");

    printf("════ Demo 7: Unary negation ════\n");
    demo_parse_and_eval("-5 + 3");

    printf("════ Demo 8: Complex expression ════\n");
    demo_parse_and_eval("(10 + 20) * 3 - 50 / (2 + 3)");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 8: Operator Precedence Explanation
 * ════════════════════════════════════════════════════════════════ */

static void demo_precedence(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 8: Operator Precedence & Associativity    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── C operator precedence (subset) ──\n\n");
    printf("  ┌───────────┬───────────────────────────┬───────────────┐\n");
    printf("  │ Prec.     │ Operators                 │ Assoc.        │\n");
    printf("  ├───────────┼───────────────────────────┼───────────────┤\n");
    printf("  │ 1 (high)  │ ()  []  ->  .             │ left-to-right │\n");
    printf("  │ 2         │ unary -  !  ~  *  &  ++   │ right-to-left │\n");
    printf("  │ 3         │ *  /  %%                    │ left-to-right │\n");
    printf("  │ 4         │ +  -                      │ left-to-right │\n");
    printf("  │ 5         │ <<  >>                    │ left-to-right │\n");
    printf("  │ 6         │ <  <=  >  >=              │ left-to-right │\n");
    printf("  │ 7         │ ==  !=                    │ left-to-right │\n");
    printf("  │ ...       │ & ^ | && || ?:            │ (varies)      │\n");
    printf("  │ 14 (low)  │ =  +=  -=  etc.          │ right-to-left │\n");
    printf("  └───────────┴───────────────────────────┴───────────────┘\n\n");

    printf("── How our parser encodes this ──\n\n");
    printf("  Each precedence level = one grammar rule:\n");
    printf("    expr   handles + -   (level 4)\n");
    printf("    term   handles * /   (level 3)\n");
    printf("    factor handles atoms (highest)\n\n");

    printf("  The deeper in the call stack → the tighter the binding.\n");
    printf("  parse_expr() calls parse_term() which calls parse_factor().\n");
    printf("  So * and / are parsed BEFORE + and -.\n\n");

    printf("── Left-to-right associativity ──\n\n");
    printf("  Expression: 10 - 3 - 2\n");
    printf("  Our parser produces: (10 - 3) - 2 = 5  (correct!)\n");
    printf("  NOT: 10 - (3 - 2) = 9  (wrong!)\n\n");
    printf("  The while-loop in parse_expr/parse_term gives us\n");
    printf("  left-associativity naturally.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 9: What GCC Does
 * ════════════════════════════════════════════════════════════════ */

static void demo_gcc_trees(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 9: Viewing GCC's Internal Trees           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("GCC builds its own AST (called GENERIC trees).\n");
    printf("You can dump them at various stages:\n\n");

    printf("  gcc -fdump-tree-original %s\n", __FILE__);
    printf("    → File: *.original  (AST right after parsing)\n\n");

    printf("  gcc -fdump-tree-gimple %s\n", __FILE__);
    printf("    → File: *.gimple  (simplified 3-address form)\n\n");

    printf("  gcc -fdump-tree-all %s\n", __FILE__);
    printf("    → Many files showing every optimisation pass!\n\n");

    printf("── Example GIMPLE for: int x = 3 + 4 * 2; ──\n\n");
    printf("  What you might see:\n");
    printf("    D.1234 = 4 * 2;        // temporary\n");
    printf("    x = 3 + D.1234;        // 3-address code\n\n");

    printf("  Or, with -O2 (constant folding):\n");
    printf("    x = 11;                // computed at compile time!\n\n");

    printf("── AST → GIMPLE → SSA → RTL → Assembly ──\n\n");
    printf("  GCC's compilation pipeline transforms the tree\n");
    printf("  through many intermediate forms, each enabling\n");
    printf("  different optimisations.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main
 * ════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 19 — Syntax Analysis (Parsing & AST)      ║\n");
    printf("║  From tokens to tree structure                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    demo_grammar_concepts();
    demo_parsing();
    demo_precedence();
    demo_gcc_trees();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: Tokens → Parser → AST → ready for semantic\n");
    printf("          analysis and code generation.\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("  Our recursive descent parser:\n");
    printf("    • One function per grammar rule\n");
    printf("    • Naturally encodes operator precedence\n");
    printf("    • Builds an AST that can be printed and evaluated\n\n");
    printf("  Next up: Chapter 20 — Semantic Analysis.\n\n");

    return 0;
}
