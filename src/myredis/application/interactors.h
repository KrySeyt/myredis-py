#ifndef INTERACTORS_H
#define INTERACTORS_H

char* ping_interactor();

char* echo_interactor(char str[]);

char* get_interactor(char key[]);

char* set_interactor(char key[], char value[], const int lifetime);

#endif //INTERACTORS_H
