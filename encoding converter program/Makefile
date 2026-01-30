CC = gcc
CFLAGS = -Wall -O2

# Целевые программы
TARGETS = utf16_to_utf8 utf8_to_utf16

all: $(TARGETS)

# Сборка utf16_to_utf8
utf16_to_utf8: utf16_to_utf8.o convert.o
	$(CC) $(CFLAGS) -o $@ utf16_to_utf8.o convert.o

# Сборка utf8_to_utf16
utf8_to_utf16: utf8_to_utf16.o convert.o
	$(CC) $(CFLAGS) -o $@ utf8_to_utf16.o convert.o

# Компиляция модулей
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Очистка
clean:
	rm -f *.o $(TARGETS)  # Удаление объектных файлов и целевых программ
	rm -f *~               # Удаление временных файлов (например, файлы с ~ на конце)
	rm -f core.*           # Удаление файлов с дампами (если они есть)

.PHONY: all clean  # Обозначение чистых целей
