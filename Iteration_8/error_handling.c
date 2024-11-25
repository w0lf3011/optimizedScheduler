#include "error_handling.h"
#include <stdio.h>

void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    // Potentially add more logic, such as error logging or recovery steps
}
