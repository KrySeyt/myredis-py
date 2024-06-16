#ifndef INTERACTORS_H
#define INTERACTORS_H

char* ping_interactor();

char* echo_interactor(char str[]);

char* get_interactor(char key[]);

char* set_interactor(char key[], char value[], const int lifetime);

int wait_interactor(const int replicas_count, const int timeout);

#endif //INTERACTORS_H
