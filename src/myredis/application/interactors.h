#ifndef INTERACTORS_H
#define INTERACTORS_H

int ping_interactor(void);

int echo_interactor(char str[]);

int get_interactor(char key[]);

int set_interactor(char key[], char value[], const int lifetime);

int wait_interactor(const int replicas_count, const int timeout);

int config_get_interactor(char key[]);

char* get_response_interactor(void);

#endif //INTERACTORS_H
