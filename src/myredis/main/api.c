#include "api.h"
#include "../application/interactors.h"

#include <stdio.h>

char* ping() {
    return ping_interactor();
}

char* echo(char str[]) {
    return echo_interactor(str);
}