// utf16_to_utf8.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert.h"

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *output_file = NULL;
    int little_endian = -1;

    // Парсим аргументы командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (input_file != NULL) {
                fprintf(stderr, "Too many arguments\n");
            }
            if (i + 1 < argc) {
                input_file = argv[++i];
            } else {
                fprintf(stderr, "Usage: utf16_to_utf8 -i input_file -o output_file [-le | -be]\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (output_file != NULL) {
                fprintf(stderr, "Too many arguments\n");
            }
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Usage: utf16_to_utf8 -i input_file -o output_file [-le | -be]\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-le") == 0) {
            if (little_endian != -1) {
                fprintf(stderr, "Too many arguments\n");
            }
            little_endian = 1;
        } else if (strcmp(argv[i], "-be") == 0) {
            if (little_endian != -1) {
                fprintf(stderr, "Too many arguments\n");
            }
            little_endian = 0;
        } else {
            fprintf(stderr, "Usage: utf16_to_utf8 -i input_file -o output_file [-le | -be]\n");
            return 1;
        }
    }

    if (little_endian == -1) {
        little_endian = 1;  // По умолчанию используем LE
    }

    // Открытие файлов
    FILE *in = input_file != NULL ? fopen(input_file, "rb") : stdin;
    if (!in) {
        fprintf(stderr, "Error: could not open input file %s\n", input_file);
        return 1;
    }

    FILE *out = input_file != NULL ? fopen(output_file, "wb") : stdout;  // Исправлено
    if (!out) {
        fprintf(stderr, "Error: could not open output file %s\n", output_file);
        fclose(in);
        return 1;
    }

    // Проверка на BOM
    if (read_bom(in, &little_endian)) {
        printf("BOM detected. Using %s-endian.\n", little_endian ? "little" : "big");
    } else {
        printf("No BOM found. Using %s-endian.\n", little_endian ? "little" : "big");
    }

    // Чтение и преобразование UTF-16 в UTF-8
    unsigned int codepoint;
    while (!feof(in)) {
        if ((codepoint = read_utf16_char(in, little_endian)) != (unsigned int)-1) {
            write_utf8(out, codepoint);
        }
    }

    // Закрытие файлов
    fclose(in);
    fclose(out);
    return 0;
}
