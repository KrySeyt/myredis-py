#include "api.h"
#include "../application/interactors.h"

char* ping() {
    return ping_interactor();
}