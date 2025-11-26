#include "lib.h" // Прототипы функций стандартной библиотеки ядра

size_t strlen(const char *str) { // Подсчёт длины строки до нулевого терминатора
    size_t len = 0; // Счётчик символов
    while (str[len] != '\0') len++; // Идём до символа конца строки
    return len; // Возвращаем длину
}

int strcmp(const char *a, const char *b) { // Лексикографическое сравнение строк
    while (*a && (*a == *b)) { // Пока символы равны и не конец строки
        a++; b++; // Сдвигаем указатели
    }
    return *(const unsigned char*)a - *(const unsigned char*)b; // Разница первых несовпавших байт
}

void *memset(void *dest, int val, size_t len) { // Заполнение памяти значением val (байтово)
    unsigned char *ptr = dest; // Итератор по байтам
    while (len-- > 0) *ptr++ = (unsigned char)val; // Заполняем len байт
    return dest; // Возвращаем начало области
}

void *memcpy(void *dest, const void *src, size_t len) { // Копирование памяти из src в dest (байтово)
    unsigned char *d = dest; // Назначение
    const unsigned char *s = src; // Источник
    while (len--) *d++ = *s++; // Копируем len байт подряд
    return dest; // Возвращаем dest для цепочек вызовов
}
