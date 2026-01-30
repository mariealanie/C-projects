#include "convert.h"

// Функция для записи символов UTF-8 в файл (уже была)
void write_utf8(FILE *out, unsigned int codepoint) {
    if (codepoint <= 0x7F) {
        fputc(codepoint, out);  // 1 байт
    } else if (codepoint <= 0x7FF) {
        fputc(0xC0 | (codepoint >> 6), out);      // 2 байта
        fputc(0x80 | (codepoint & 0x3F), out);
    } else if (codepoint <= 0xFFFF) {
        fputc(0xE0 | (codepoint >> 12), out);     // 3 байта
        fputc(0x80 | ((codepoint >> 6) & 0x3F), out);
        fputc(0x80 | (codepoint & 0x3F), out);
    } else {
        fputc(0xF0 | (codepoint >> 18), out);     // 4 байта
        fputc(0x80 | ((codepoint >> 12) & 0x3F), out);
        fputc(0x80 | ((codepoint >> 6) & 0x3F), out);
        fputc(0x80 | (codepoint & 0x3F), out);
    }
}

// Функция для записи UTF-16 символов с учётом порядка байтов
void write_utf16(FILE *out, unsigned int codepoint, int little_endian) {
    if (codepoint <= 0xFFFF) {
        unsigned short wc = codepoint;
        if (!little_endian) {
            wc = (wc >> 8) | (wc << 8);  // Меняем порядок байтов
        }
        fwrite(&wc, sizeof(wc), 1, out);
    } else {
        // Суррогатная пара для кодовых точек выше U+FFFF
        unsigned short high_surrogate = 0xD800 + ((codepoint - 0x10000) >> 10);
        unsigned short low_surrogate = 0xDC00 + (codepoint & 0x3FF);

        if (!little_endian) {
            high_surrogate = (high_surrogate >> 8) | (high_surrogate << 8);
            low_surrogate = (low_surrogate >> 8) | (low_surrogate << 8);
        }
        fwrite(&high_surrogate, sizeof(high_surrogate), 1, out);
        fwrite(&low_surrogate, sizeof(low_surrogate), 1, out);
    }
}

// Функция для чтения UTF-8 символов (похожая на write_utf8, но наоборот)
unsigned int read_utf8_char(FILE *in) {
    int c = fgetc(in);
    if (c == EOF) {
        return (unsigned int)-1;  // Конец файла
    }

    unsigned int codepoint;

    if (c <= 0x7F) {
        // Однобайтовый символ
        return c;
    } else if ((c & 0xE0) == 0xC0) {
        // Двухбайтовый символ
        int c2 = fgetc(in);
        if (c2 == EOF) return (unsigned int)-1;
        codepoint = ((c & 0x1F) << 6) | (c2 & 0x3F);
    } else if ((c & 0xF0) == 0xE0) {
        // Трехбайтовый символ
        int c2 = fgetc(in);
        int c3 = fgetc(in);
        if (c2 == EOF || c3 == EOF) return (unsigned int)-1;
        codepoint = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
    } else if ((c & 0xF8) == 0xF0) {
        // Четырехбайтовый символ
        int c2 = fgetc(in);
        int c3 = fgetc(in);
        int c4 = fgetc(in);
        if (c2 == EOF || c3 == EOF || c4 == EOF) return (unsigned int)-1;
        codepoint = ((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
    } else {
        // Неверный UTF-8 символ
        fprintf(stderr, "Error: invalid UTF-8 byte: %c. Offset: %ld.\n", c, ftell(in) - sizeof(c));
        return (unsigned int)-1;
    }

    return codepoint;
}

// Функция для чтения BOM и определения порядка байтов
int read_bom(FILE *in, int *little_endian) {
    unsigned char bom1, bom2;
    size_t read_size1 = fread(&bom1, sizeof(bom1), 1, in);
    size_t read_size2 = fread(&bom2, sizeof(bom2), 1, in);

    if (read_size1 != 1 || read_size2 != 1) {
        return 0; // Ошибка или конец файла
    }

    // Устанавливаем порядок байтов в зависимости от прочитанного BOM
    if (bom1 == 0xFF && bom2 == 0xFE) {
        *little_endian = 1; // Little-endian
    } else if (bom1 == 0xFE && bom2 == 0xFF) {
        *little_endian = 0; // Big-endian
    } else {
        return 0; // BOM не распознан, возвращаем 0
    }

    return 1; // BOM успешно прочитан
}

unsigned int read_utf16_char(FILE *in, int little_endian) {
    unsigned short wc;
    size_t read_size = fread(&wc, sizeof(wc), 1, in);

    if (read_size != 1) {
        if (feof(in)) {
            return (unsigned int)-1; // Конец файла
        } else {
            long offset = ftell(in);
            fprintf(stderr, "Error: read error at offset %ld\n", offset);
            return (unsigned int)-1; // Ошибка чтения
        }
    }

    // Меняем порядок байтов, если необходимо
    if (!little_endian) {
        wc = (wc >> 8) | (wc << 8);
    }

    // Проверяем на суррогатные пары
    if (wc >= 0xD800 && wc <= 0xDBFF) {
        // Высокая часть суррогатной пары
        unsigned short low_wc;
        read_size = fread(&low_wc, sizeof(low_wc), 1, in);

        if (read_size != 1) {
            long offset = ftell(in);
            fprintf(stderr, "Error: incomplete surrogate pair at offset %ld, code: 0x%04X\n", offset, wc);
            return (unsigned int)-1; // Ошибка чтения
        }

        if (!little_endian) {
            low_wc = (low_wc >> 8) | (low_wc << 8);
        }

        // Проверяем, что нижняя часть суррогатной пары корректна
        if (low_wc < 0xDC00 || low_wc > 0xDFFF) {
            long offset = ftell(in) - sizeof(low_wc);
            fprintf(stderr, "Error: invalid low surrogate at offset %ld, code: 0x%04X\n", offset, low_wc);
            return (unsigned int)-1; // Некорректная нижняя часть суррогатной пары
        }

        // Формируем кодовую точку из суррогатной пары
        return 0x10000 + ((wc - 0xD800) << 10) + (low_wc - 0xDC00);
    } else if (wc >= 0xDC00 && wc <= 0xDFFF) {
        // Некорректная высокая часть суррогатной пары
        long offset = ftell(in) - sizeof(wc);
        fprintf(stderr, "Error: invalid high surrogate at offset %ld, code: 0x%04X\n", offset, wc);
        return (unsigned int)-1;
    }

    return wc; // Возвращаем обычный символ
}
