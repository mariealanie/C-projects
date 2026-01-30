#ifndef CONVERT_H
#define CONVERT_H

#include <stdio.h>

// Функция для записи символов UTF-8 в файл
void write_utf8(FILE *out, unsigned int codepoint);

// Функция для чтения BOM и определения порядка байтов
int read_bom(FILE *in, int *little_endian);

// Функция для чтения символов UTF-16 с учётом порядка байтов
unsigned int read_utf16_char(FILE *in, int little_endian);

// Новая функция для записи символов UTF-16
void write_utf16(FILE *out, unsigned int codepoint, int little_endian);

// Функция для чтения UTF-8 символов
unsigned int read_utf8_char(FILE *in);

#endif  // CONVERT_H
