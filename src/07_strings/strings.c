/*
 * Chapter 7 — Strings
 *
 * C has no built-in string type.  A "string" is just a char array
 * terminated by '\0' (the null character, value 0).  This chapter covers:
 *   1. String basics — literals, arrays, null terminator
 *   2. Manipulation — strcpy, strcat, and their safer 'n' variants
 *   3. Searching — strchr, strstr, strtok
 *   4. Comparison — strcmp, strncmp, case-insensitive
 *   5. Conversion — atoi, strtol (and why strtol wins)
 *   6. Formatted output — sprintf, snprintf
 *   7. Common pitfalls — buffer overflows, missing '\0', UB
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/07_strings \
 *            src/07_strings/strings.c
 * Run:   ./bin/07_strings
 *
 * Try these:
 *   - Change buf[6] to buf[4] in demo_manipulation — what happens?
 *   - Call strtok on a string literal — why does it crash?
 *   - Pass "99xyz" to atoi vs strtol — compare error handling
 *   - Compile with -fsanitize=address and trigger a buffer overflow
 */

#include "../../include/common.h"
#include <ctype.h>     /* tolower, toupper */
#include <strings.h>   /* strcasecmp (POSIX) */

/* ════════════════════════════════════════════════════════════════
 *  Section 1: String Basics
 *  Literals, char arrays, null terminator, sizeof vs strlen.
 * ════════════════════════════════════════════════════════════════ */

static void demo_basics(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: String Basics                          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* A string literal lives in read-only memory                    */
    const char *literal = "Hello";

    /* A char array is a mutable copy of the literal                 */
    char array[] = "Hello";    /* compiler adds '\0' → 6 bytes       */

    printf("  const char *literal = \"Hello\";\n");
    printf("  char array[]        = \"Hello\";\n\n");

    /* The critical difference: sizeof vs strlen                     */
    printf("  sizeof(array)  = %zu   ← includes '\\0'\n", sizeof(array));
    printf("  strlen(array)  = %zu     ← counts characters, stops at '\\0'\n", strlen(array));
    printf("  sizeof(literal)= %zu     ← size of a POINTER, not the string!\n\n", sizeof(literal));

    /* Peek at the null terminator                                   */
    printf("  Memory layout of \"Hello\":\n  ");
    for (size_t i = 0; i <= strlen(array); i++) {
        if (array[i] == '\0')
            printf(" ['\\0']");
        else
            printf(" ['%c']", array[i]);
    }
    printf("\n  index: 0    1    2    3    4    5\n\n");

    printf("  What happens: every string function walks forward until\n");
    printf("  it hits '\\0'.  Forget that terminator → buffer overrun.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: String Manipulation
 *  strcpy/strncpy, strcat/strncat — and why 'n' variants exist.
 * ════════════════════════════════════════════════════════════════ */

static void demo_manipulation(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: String Manipulation                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    char dest[32];

    /* strcpy: copies until '\0' — no bounds checking!               */
    strcpy(dest, "Hello");
    printf("  strcpy(dest, \"Hello\")       → \"%s\"\n", dest);

    /* strncpy: copies at most n chars — safer but tricky            */
    char safe[6];
    strncpy(safe, "Hello World", sizeof(safe) - 1);
    safe[sizeof(safe) - 1] = '\0';   /* strncpy may NOT add '\0'!   */
    printf("  strncpy(safe, \"Hello World\", 5) → \"%s\"  (truncated)\n", safe);

    /* strcat: appends — dest must have room!                        */
    strcpy(dest, "Hello");
    strcat(dest, " World");
    printf("  strcat → \"%s\"\n", dest);

    /* strncat: safer — limits how many chars to append              */
    char buf[16] = "Hi";
    strncat(buf, " there, friend!", sizeof(buf) - strlen(buf) - 1);
    printf("  strncat → \"%s\"  (truncated to fit)\n\n", buf);

    printf("  Rule of thumb: Always prefer strn* variants.\n");
    printf("  strncpy gotcha: if src >= n, NO '\\0' is written!\n");
    printf("  strncat gotcha: the 'n' does NOT include '\\0'.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Searching Strings
 *  strchr, strrchr, strstr, and the infamous strtok.
 * ════════════════════════════════════════════════════════════════ */

static void demo_searching(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Searching Strings                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    const char *path = "/home/user/docs/file.txt";

    /* strchr: find first occurrence of a character                   */
    const char *dot = strchr(path, '.');
    printf("  path = \"%s\"\n", path);
    printf("  strchr(path, '.')   → \"%s\"  (from first '.')\n", dot ? dot : "(null)");

    /* strrchr: find LAST occurrence — great for file extensions     */
    const char *last_slash = strrchr(path, '/');
    printf("  strrchr(path, '/')  → \"%s\"   (filename)\n", last_slash ? last_slash + 1 : path);

    /* strstr: find a substring                                      */
    const char *haystack = "The quick brown fox jumps";
    const char *found = strstr(haystack, "brown");
    printf("  strstr(\"...quick brown fox...\", \"brown\") → \"%s\"\n\n", found ? found : "(null)");

    /* strtok: splits string IN PLACE — modifies the original!       */
    char csv[] = "Alice,Bob,Charlie,Diana";
    printf("  strtok on \"%s\":\n", "Alice,Bob,Charlie,Diana");
    char *token = strtok(csv, ",");
    int idx = 0;
    while (token) {
        printf("    token[%d] = \"%s\"\n", idx++, token);
        token = strtok(NULL, ",");  /* NULL = continue from last pos */
    }
    printf("\n  Warning: strtok uses static state → NOT thread-safe.\n");
    printf("  It also modifies the source string (replaces ',' with '\\0').\n");
    printf("  Prefer strtok_r (POSIX) for thread-safe tokenizing.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: String Comparison
 *  strcmp, strncmp, and case-insensitive comparison.
 * ════════════════════════════════════════════════════════════════ */

static void demo_comparison(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: String Comparison                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* strcmp returns: <0, 0, or >0 — NOT just -1/0/1!               */
    printf("  strcmp(\"abc\", \"abc\") = %d   (equal)\n", strcmp("abc", "abc"));
    printf("  strcmp(\"abc\", \"abd\") = %d   (c < d → negative)\n", strcmp("abc", "abd"));
    printf("  strcmp(\"abd\", \"abc\") = %d    (d > c → positive)\n\n", strcmp("abd", "abc"));

    /* strncmp: compare only first n characters                      */
    printf("  strncmp(\"Hello World\", \"Hello There\", 5) = %d  (first 5 match)\n",
           strncmp("Hello World", "Hello There", 5));
    printf("  strncmp(\"Hello World\", \"Hello There\", 7) = %d  (differ at [6])\n\n",
           strncmp("Hello World", "Hello There", 7));

    /* Case-insensitive comparison (POSIX, not standard C)           */
    printf("  strcasecmp(\"Hello\", \"HELLO\") = %d  (POSIX extension)\n\n",
           strcasecmp("Hello", "HELLO"));

    printf("  Common bug: if (str1 == str2) compares POINTERS,\n");
    printf("  not content.  Always use strcmp() for string equality.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: String ↔ Number Conversion
 *  atoi, atof, strtol — and why strtol is preferred.
 * ════════════════════════════════════════════════════════════════ */

static void demo_conversion(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: String ↔ Number Conversion             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* atoi: simple but dangerous — no error detection               */
    printf("  atoi(\"42\")       = %d\n", atoi("42"));
    printf("  atoi(\"99xyz\")    = %d   ← silently stops at 'x'\n", atoi("99xyz"));
    printf("  atoi(\"hello\")    = %d     ← returns 0 (same as atoi(\"0\")!)\n\n", atoi("hello"));

    /* strtol: proper error detection via endptr and errno           */
    char *endptr;
    errno = 0;
    long val = strtol("99xyz", &endptr, 10);
    printf("  strtol(\"99xyz\", &endptr, 10):\n");
    printf("    value   = %ld\n", val);
    printf("    endptr  = \"%s\"   ← tells you where parsing stopped\n", endptr);
    printf("    errno   = %d     ← 0 means no overflow/underflow\n\n", errno);

    /* strtol with different bases                                   */
    printf("  strtol(\"0xFF\", NULL, 16) = %ld  (hex)\n", strtol("0xFF", NULL, 16));
    printf("  strtol(\"0b1010\", NULL, 0) may not work — base 0 auto-detects\n");
    printf("  strtol(\"0777\", NULL, 0)  = %ld  (octal, auto-detected)\n\n", strtol("0777", NULL, 0));

    printf("  Rule: never use atoi() in production code.\n");
    printf("  strtol gives you: overflow detection, parse-stop position,\n");
    printf("  and base control.  Use it.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Formatted String Output
 *  sprintf, snprintf — building strings safely.
 * ════════════════════════════════════════════════════════════════ */

static void demo_formatting(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Formatted String Output                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    char buf[64];

    /* sprintf: like printf, but writes to a string                  */
    sprintf(buf, "Pi ≈ %.4f, answer = %d", 3.14159, 42);
    printf("  sprintf → \"%s\"\n", buf);

    /* snprintf: limits output to n-1 chars + '\0' — ALWAYS use this */
    char small[16];
    int needed = snprintf(small, sizeof(small), "A very long string that won't fit");
    printf("  snprintf(small, 16, \"A very long...\") → \"%s\"\n", small);
    printf("  Return value = %d (chars that WOULD have been written)\n", needed);
    printf("  If return >= sizeof(buf), output was truncated.\n\n");

    /* Building strings incrementally                                */
    char msg[64];
    int offset = 0;
    offset += snprintf(msg + offset, sizeof(msg) - offset, "Temp: %d°C", 23);
    offset += snprintf(msg + offset, sizeof(msg) - offset, ", Humid: %d%%", 65);
    printf("  Incremental build → \"%s\"\n\n", msg);

    printf("  Rule: sprintf is banned in safety-critical code.\n");
    printf("  Always use snprintf with sizeof(buffer).\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Common String Pitfalls
 *  Buffer overflow, missing '\0', modifying literals.
 * ════════════════════════════════════════════════════════════════ */

static void demo_common_pitfalls(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Common String Pitfalls                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* 1. Buffer overflow with strcpy                                */
    printf("  1) Buffer overflow:\n");
    printf("     char buf[5];\n");
    printf("     strcpy(buf, \"Hello World\");  // writes 12 bytes into 5!\n");
    printf("     → stack smashing, crashes, security exploits.\n");
    printf("     Fix: use strncpy or snprintf with sizeof.\n\n");

    /* 2. Missing null terminator                                    */
    printf("  2) Missing null terminator:\n");
    printf("     char name[5];\n");
    printf("     strncpy(name, \"Hello\", 5);  // no room for '\\0'!\n");
    printf("     printf(\"%%s\", name);  // reads past array → UB\n");
    printf("     Fix: name[sizeof(name)-1] = '\\0'; or use size-1.\n\n");

    /* 3. Modifying a string literal — undefined behavior            */
    printf("  3) Modifying a string literal:\n");
    printf("     char *s = \"Hello\";  // points to read-only memory\n");
    printf("     s[0] = 'h';          // UNDEFINED BEHAVIOR!\n");
    printf("     Some systems: segfault. Others: silently corrupt.\n");
    printf("     Fix: use char s[] = \"Hello\"; for mutable strings.\n\n");

    /* 4. Comparing strings with ==                                  */
    printf("  4) Using == to compare strings:\n");
    printf("     if (name == \"Alice\")  // compares addresses, not content!\n");
    printf("     Fix: if (strcmp(name, \"Alice\") == 0)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main — run all demos in order
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 7 — Strings                 v%s        ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════╝\n");

    demo_basics();
    demo_manipulation();
    demo_searching();
    demo_comparison();
    demo_conversion();
    demo_formatting();
    demo_common_pitfalls();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: C strings are just char arrays with '\\0'.\n");
    printf(" Always know your buffer size, always null-terminate,\n");
    printf(" and prefer the 'n' variants (strncpy, snprintf).\n");
    printf("════════════════════════════════════════════════════════\n");

    DEMO_END();
    return 0;
}
