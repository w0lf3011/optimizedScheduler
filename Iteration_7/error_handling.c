#include "error_handling.h"
#include <stdio.h>

/**
 * \brief Handles errors by logging an error message and terminating the program.
 * 
 * This function logs the specified error message to the standard error output.
 * It is intended for critical errors that require program termination.
 * 
 * \param message A string containing the error message to be displayed.
 */
void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    // Potentially add more logic, such as error logging or recovery steps
}
