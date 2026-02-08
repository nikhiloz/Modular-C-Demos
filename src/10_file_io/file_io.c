/*
 * Chapter 10 — File I/O
 *
 * Everything in Unix is a file — and stdio.h gives you a portable
 * interface to read and write them.  This chapter covers:
 *   1. Text I/O — fopen, fprintf, fgets, fclose
 *   2. Binary I/O — fread, fwrite, struct serialization
 *   3. File seeking — fseek, ftell, rewind
 *   4. Buffering — setvbuf, fflush, line vs full vs none
 *   5. Error handling — ferror, feof, perror, errno
 *   6. Formatted input — fscanf patterns and format string safety
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/10_file_io \
 *            src/10_file_io/file_io.c
 * Run:   ./bin/10_file_io
 *
 * Try these:
 *   - Change /tmp/demo_*.* paths to a non-writable dir — check errors
 *   - Set setvbuf to _IONBF on stdout and watch output change
 *   - Use hexdump -C /tmp/demo_binary.bin to inspect the binary file
 *   - Open a file with "a" mode and run twice — what happens?
 */

#include "../../include/common.h"

/* Temp file paths — using /tmp so demos are safe to run anywhere    */
#define TEXT_FILE   "/tmp/demo_text.txt"
#define BINARY_FILE "/tmp/demo_binary.bin"

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Text File I/O
 *  fopen modes, fprintf, fgets, line-by-line reading.
 * ════════════════════════════════════════════════════════════════ */

static void demo_text_io(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Text File I/O                          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* fopen modes: "r" read, "w" write (truncate), "a" append       */
    /*              "r+" read/write, "w+" write/read, "a+" append/read */
    FILE *f = fopen(TEXT_FILE, "w");
    if (!f) {
        perror("fopen(write)"); /* perror prints: "fopen(write): reason" */
        return;
    }

    /* fprintf works exactly like printf but targets a file          */
    fprintf(f, "Line 1: Hello from C file I/O!\n");
    fprintf(f, "Line 2: The answer is %d\n", 42);
    fprintf(f, "Line 3: Pi ≈ %.5f\n", 3.14159);
    fclose(f);                 /* ALWAYS close — flushes and releases */
    printf("  Wrote 3 lines to %s\n\n", TEXT_FILE);

    /* Reading line by line with fgets                               */
    f = fopen(TEXT_FILE, "r");
    if (!f) { perror("fopen(read)"); return; }

    char line[128];
    int line_num = 0;
    printf("  Reading back:\n");
    while (fgets(line, sizeof(line), f)) {
        /* fgets keeps the '\n' — be aware of that                   */
        printf("    [%d] %s", ++line_num, line);
    }
    printf("\n");
    fclose(f);

    printf("  fgets(buf, size, fp) reads up to size-1 chars or until '\\n'.\n");
    printf("  It keeps the '\\n'. Returns NULL at EOF or on error.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Binary File I/O
 *  fread, fwrite — raw bytes, no text conversion.
 * ════════════════════════════════════════════════════════════════ */

typedef struct {
    char name[16];
    int  score;
} Record;

static void demo_binary_io(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Binary File I/O                        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Writing structs directly — no formatting, raw bytes           */
    Record records[] = {
        {"Alice",   95},
        {"Bob",     87},
        {"Charlie", 92}
    };
    size_t count = ARRAY_SIZE(records);

    FILE *f = fopen(BINARY_FILE, "wb");   /* "wb" = write binary     */
    if (!f) { perror("fopen(wb)"); return; }
    size_t written = fwrite(records, sizeof(Record), count, f);
    fclose(f);
    printf("  Wrote %zu records (%zu bytes each) to %s\n",
           written, sizeof(Record), BINARY_FILE);

    /* Reading them back                                             */
    Record buf[3];
    f = fopen(BINARY_FILE, "rb");         /* "rb" = read binary      */
    if (!f) { perror("fopen(rb)"); return; }
    size_t read_count = fread(buf, sizeof(Record), count, f);
    fclose(f);

    printf("  Read back %zu records:\n", read_count);
    for (size_t i = 0; i < read_count; i++)
        printf("    %-10s score=%d\n", buf[i].name, buf[i].score);
    printf("\n");

    printf("  Warning: binary files are NOT portable across different\n");
    printf("  architectures (endianness) or compilers (struct padding).\n");
    printf("  Use text/JSON/protobuf for portable data exchange.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: File Seeking
 *  fseek, ftell, rewind — random access within files.
 * ════════════════════════════════════════════════════════════════ */

static void demo_fseek(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: File Seeking                           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    FILE *f = fopen(TEXT_FILE, "r");
    if (!f) { perror("fopen"); return; }

    /* Get file size by seeking to end                               */
    fseek(f, 0, SEEK_END);    /* SEEK_END = from end of file        */
    long size = ftell(f);      /* ftell returns current position     */
    printf("  File size: %ld bytes\n", size);

    /* Rewind to beginning                                           */
    rewind(f);                 /* equivalent to fseek(f, 0, SEEK_SET) */
    char first = (char)fgetc(f);
    printf("  First char: '%c'\n", first);

    /* Seek to a specific offset                                     */
    fseek(f, 10, SEEK_SET);   /* SEEK_SET = from beginning          */
    char at_10 = (char)fgetc(f);
    printf("  Char at offset 10: '%c'\n", at_10);

    /* Relative seek                                                 */
    fseek(f, -5, SEEK_CUR);   /* SEEK_CUR = from current position   */
    long pos = ftell(f);
    printf("  After fseek(-5, SEEK_CUR): position = %ld\n\n", pos);

    printf("  fseek origins: SEEK_SET (start), SEEK_CUR (current), SEEK_END (end)\n");
    printf("  For binary files, offsets are exact byte counts.\n");
    printf("  For text files, only ftell() return values are portable offsets.\n\n");

    fclose(f);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Buffering Modes
 *  setvbuf, fflush — controlling when data actually gets written.
 * ════════════════════════════════════════════════════════════════ */

static void demo_buffering(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Buffering Modes                        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  Three buffering modes (set with setvbuf):\n\n");
    printf("  _IOFBF (full):    flush when buffer is full.\n");
    printf("                    Default for files.\n");
    printf("  _IOLBF (line):    flush at every '\\n'.\n");
    printf("                    Default for stdout when connected to terminal.\n");
    printf("  _IONBF (none):    write every byte immediately.\n");
    printf("                    Default for stderr.\n\n");

    /* Demonstrating fflush                                          */
    FILE *f = fopen("/tmp/demo_buffer.txt", "w");
    if (!f) { perror("fopen"); return; }

    /* Set full buffering with a 256-byte buffer                     */
    char mybuf[256];
    setvbuf(f, mybuf, _IOFBF, sizeof(mybuf));

    fprintf(f, "This is buffered...");
    /* Data is in the buffer, NOT on disk yet                        */
    printf("  After fprintf: data is in memory buffer, not on disk.\n");

    fflush(f);
    /* NOW it's on disk                                              */
    printf("  After fflush:  data is written to disk.\n\n");

    fprintf(f, "Final line.\n");
    fclose(f);                 /* fclose always flushes first         */

    printf("  fclose() calls fflush() automatically.\n");
    printf("  But crash before fclose → buffered data may be lost!\n");
    printf("  For critical data: fflush() after each write.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Error Handling
 *  ferror, feof, perror, errno — knowing what went wrong.
 * ════════════════════════════════════════════════════════════════ */

static void demo_error_handling(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Error Handling                         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Try opening a non-existent file                               */
    FILE *f = fopen("/nonexistent/path/file.txt", "r");
    if (!f) {
        printf("  fopen failed:\n");
        printf("    errno  = %d\n", errno);
        printf("    perror → ");
        perror("fopen");       /* prints: "fopen: No such file or directory" */
        printf("\n");
    }

    /* ferror vs feof: two different end conditions                  */
    f = fopen(TEXT_FILE, "r");
    if (f) {
        char buf[1024];
        /* Read until we can't read anymore                          */
        while (fgets(buf, sizeof(buf), f)) { /* consume */ }

        /* After the loop: WHY did we stop?                          */
        if (feof(f))
            printf("  feof(f) = true:   stopped because end-of-file reached.\n");
        if (ferror(f))
            printf("  ferror(f) = true: stopped because of an I/O error.\n");

        printf("  Always check BOTH feof and ferror after a read loop.\n\n");

        clearerr(f);           /* reset error/EOF indicators         */
        printf("  clearerr(f): resets both feof and ferror flags.\n\n");
        fclose(f);
    }

    printf("  errno gotcha: it's only meaningful immediately after\n");
    printf("  a failed call.  Successful calls may NOT reset it.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Formatted Input (fscanf)
 *  Parsing patterns and format string safety.
 * ════════════════════════════════════════════════════════════════ */

static void demo_formatted_io(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Formatted Input (fscanf)               ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Write a structured file to parse back                         */
    FILE *f = fopen("/tmp/demo_formatted.txt", "w");
    if (!f) { perror("fopen"); return; }
    fprintf(f, "Alice 95\nBob 87\nCharlie 92\n");
    fclose(f);

    /* fscanf parses formatted data                                  */
    f = fopen("/tmp/demo_formatted.txt", "r");
    if (!f) { perror("fopen"); return; }

    char name[32];
    int score;
    printf("  Parsing \"name score\" lines with fscanf:\n");
    /* %31s = max 31 chars (leaves room for '\0') — ALWAYS limit!    */
    while (fscanf(f, "%31s %d", name, &score) == 2) {
        printf("    %-10s %d\n", name, score);
    }
    fclose(f);
    printf("\n");

    /* The dangers of scanf/fscanf                                   */
    printf("  fscanf/scanf safety rules:\n");
    printf("    1. ALWAYS limit string width: %%31s not %%s\n");
    printf("       %%s without width limit → buffer overflow!\n");
    printf("    2. Check return value: fscanf returns number of\n");
    printf("       successfully matched items, or EOF on failure.\n");
    printf("    3. %%s stops at whitespace — can't read full lines.\n");
    printf("       Use fgets + sscanf for complex parsing.\n\n");

    printf("  Format string vulnerability:\n");
    printf("    char *user_input = get_input();\n");
    printf("    printf(user_input);  // NEVER DO THIS!\n");
    printf("    // Attacker can use %%x, %%n to read/write memory.\n");
    printf("    printf(\"%%s\", user_input);  // Safe: format is fixed.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main — run all demos in order
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 10 — File I/O               v%s        ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════╝\n");

    demo_text_io();
    demo_binary_io();
    demo_fseek();
    demo_buffering();
    demo_error_handling();
    demo_formatted_io();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: stdio.h gives you text (fprintf/fgets) and\n");
    printf(" binary (fread/fwrite) I/O, buffering control, seeking,\n");
    printf(" and error detection.  Always check returns and close files.\n");
    printf("════════════════════════════════════════════════════════\n");

    DEMO_END();
    return 0;
}
