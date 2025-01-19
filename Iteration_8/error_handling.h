#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

/**
 * \brief Handles errors by displaying an error message.
 * 
 * This function outputs the provided error message to the standard error stream.
 * It serves as a central mechanism for reporting errors and can be extended to
 * include logging or recovery mechanisms.
 * 
 * \param message The error message to be handled and displayed.
 * 
 * \note Additional error handling features, such as logging to a file or triggering
 * recovery processes, can be implemented in the function body.
 */
void handle_error(const char *message);

#endif // ERROR_HANDLING_H
