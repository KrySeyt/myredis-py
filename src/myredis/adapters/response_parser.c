#include "response_parser.h"

#include <stdio.h>
#include <stdlib.h>

char* parse_response(char response[]) {
    char *parsed_response = malloc(200);
    char *curr_resp = response;
    char *curr_parsed = parsed_response;
    switch (response[0]) {
        case '+': case ':':
            while (*++curr_resp != '\r') {
                *(curr_parsed++) = *curr_resp;
            }

            *(curr_parsed) = '\0';

            return parsed_response;

        case '$':
            if (response[1] == '-' && response[2] == '1') {
                return NULL;
            }

            while (*curr_resp != '\n')
                curr_resp++;

            while (*(++curr_resp) != '\r') {
                *(curr_parsed++) = *curr_resp;
            }

            *(curr_parsed) = '\0';

            return parsed_response;

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

            return parsed_response;

        default:
            printf("%s", response);
            return "";
    }
}
