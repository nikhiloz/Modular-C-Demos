# Chapter 19 — Parsing and Abstract Syntax Trees

## Overview
Parsing takes the flat stream of tokens produced by the lexer and organises them
into a hierarchical structure that reflects the grammatical rules of the
language. This chapter implements a recursive-descent parser for arithmetic
expressions, constructs an Abstract Syntax Tree (AST), and evaluates the result.
Along the way you'll learn BNF grammar notation, operator precedence, and how
real compilers represent program structure in memory.

## Key Concepts
- BNF and EBNF grammar notation for defining language syntax
- Recursive descent parsing as a top-down parsing strategy
- AST construction: nodes, children, and tree traversal
- Expression evaluation by walking the AST
- Operator precedence and associativity encoded in grammar rules
- Handling parenthesised sub-expressions
- Error reporting during parsing
- The difference between parse trees (CSTs) and abstract syntax trees (ASTs)

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | What Is Parsing | Turning a token stream into structured syntax |
| 2 | BNF Grammar Notation | Formal rules for describing expression syntax |
| 3 | Recursive Descent Parsing | One function per grammar rule, top-down approach |
| 4 | Building the AST | Allocating nodes and linking them into a tree |
| 5 | AST Node Types | Number literals, binary operators, unary operators |
| 6 | Expression Evaluation | Recursive tree walk to compute a numeric result |
| 7 | Precedence and Associativity | How grammar layering enforces `*` before `+` |
| 8 | Demo Expressions | Sample inputs and their resulting ASTs |

## Building & Running
```bash
make bin/19_parsing_ast
./bin/19_parsing_ast
```

## Diagrams
- [Concept Diagram](19_parsing_ast_concept.png)
- [Code Flow Diagram](19_parsing_ast_flow.png)

## Try It Yourself
```bash
# Run the demo to see AST building and evaluation
./bin/19_parsing_ast

# Try different expressions (if interactive mode is supported)
echo "3 + 4 * 2" | ./bin/19_parsing_ast
echo "(3 + 4) * 2" | ./bin/19_parsing_ast
echo "10 - 2 - 3" | ./bin/19_parsing_ast    # left-associativity check

# Visualise the AST structure
# The demo prints a textual tree; compare simple vs complex expressions:
echo "1 + 2" | ./bin/19_parsing_ast
echo "1 + 2 * 3 - 4 / 2" | ./bin/19_parsing_ast

# Experiment: extend the grammar to support a new operator (e.g., %)
# Edit src/19_parsing_ast/parsing_ast.c, then:
make bin/19_parsing_ast && ./bin/19_parsing_ast
```

## Further Reading
- *Compilers: Principles, Techniques, and Tools* (Aho, Lam, Sethi, Ullman) — Chapter 4: Syntax Analysis
- *Crafting Interpreters* (Robert Nystrom) — [Chapter 6: Parsing Expressions](https://craftinginterpreters.com/parsing-expressions.html)
- *Writing An Interpreter In Go* (Thorsten Ball) — Chapters 2–3: Parsing
