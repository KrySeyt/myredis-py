#ifndef INTERACTORS_H
#define INTERACTORS_H

int ping_interactor(void);

int echo_interactor(const char str[]);

int get_interactor(const char key[]);

int set_interactor(const char key[], const char value[], const int lifetime);

int wait_interactor(const int replicas_count, const int timeout);

int config_get_interactor(const char key[]);

int get_response_interactor(const char* out);

#endif //INTERACTORS_H
