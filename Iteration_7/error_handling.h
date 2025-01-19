#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

/**
 * \brief Handles errors by displaying an error message and terminating the program.
 * 
 * This function logs the provided error message to standard error output and exits
 * the program with a failure status. It is intended to handle critical errors that
 * cannot be recovered from during program execution.
 * 
 * \param message A string containing the error message to be displayed.
 */
void handle_error(const char *message);

#endif // ERROR_HANDLING_H
