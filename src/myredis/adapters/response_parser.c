#include "response_parser.h"

#include <stdio.h>
#include <stdlib.h>

void parse_response(char response[], char *out) {
    char *curr_resp = response;
    char *curr_parsed = out;
    switch (response[0]) {
        case '+': case ':':
            while (*++curr_resp != '\r') {
                *(curr_parsed++) = *curr_resp;
            }

            *(curr_parsed) = '\0';

            return;

        case '$':
            if (response[1] == '-' && response[2] == '1') {
                return;
            }

            while (*curr_resp != '\n')
                curr_resp++;

            while (*(++curr_resp) != '\r') {
                *(curr_parsed++) = *curr_resp;
            }

            *(curr_parsed) = '\0';

            return;

        case '*':
            for (int i = 0; i < 4; i++) {
                while (*curr_resp != '\n') {
                    curr_resp++;
                }
                curr_resp++;
            }


            while (*curr_resp != '\r') {
                *(curr_parsed++) = *(curr_resp++);
            }

            *curr_parsed = '\0';

            return;

        default:
            printf("%s", response);
            return;
    }
}
