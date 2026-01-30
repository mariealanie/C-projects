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

    int temp_bom = 0;
    // Игнорирование BOM для utf-8
    if (!read_bom(in, &temp_bom)) {
        fseek(in, 0, SEEK_SET);
    }

    // Записать BOM для UTF-16
    unsigned short bom = little_endian ? 0xFFFE : 0xFEFF;
    fwrite(&bom, sizeof(bom), 1, out);

    // Чтение и преобразование UTF-8 в UTF-16
    unsigned int codepoint;
    while (!feof(in)) {
        if ((codepoint = read_utf8_char(in)) != (unsigned int)-1) {
            write_utf16(out, codepoint, little_endian);
        }
    }

    // Закрытие файлов
    fclose(in);
    fclose(out);
    return 0;
}
