#include "api.h"
#include "../application/interactors.h"

#include <stdio.h>


void print(char str[]) {
    for (; *str != '\0'; str++) {
        if (*str == '\n') {
            putchar('\\');
            putchar('n');
        }

        else if (*str == '\r') {
            putchar('\\');
            putchar('r');
        }
        else {
            putchar(*str);
        }
    }

    putchar('\n');
}


char* ping() {
    return ping_interactor();
}

char* echo(char str[]) {
    char *v = echo_interactor(str);
    print(v);
    return v;
}

char* get(char key[]) {
    return get_interactor(key);
}

char* set(char key[], char value[]) {
    return set_interactor(key, value);
}