#ifndef INTERACTORS_H
#define INTERACTORS_H

void ping_interactor(void);

void echo_interactor(char str[]);

void get_interactor(char key[]);

void set_interactor(char key[], char value[], const int lifetime);

void wait_interactor(const int replicas_count, const int timeout);

void config_get_interactor(char key[]);

char* get_response_interactor(void);

#endif //INTERACTORS_H
