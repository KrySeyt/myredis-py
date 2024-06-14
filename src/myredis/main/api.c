#include "api.h"
#include "../application/interactors.h"

#include <stdio.h>

char* ping() {
    return ping_interactor();
}

char* echo(char str[]) {
    return echo_interactor(str);
}

char* get(char key[]) {
    return get_interactor(key);
}

char* set(char key[], char value[]) {
    return set_interactor(key, value);
}