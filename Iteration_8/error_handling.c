#include "error_handling.h"
#include <stdio.h>

void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    // Log errors to a file for post-mortem analysis -- To adapt based on the system
    //FILE *log_file = fopen("error_log.txt", "a");
    //if (log_file) {
    //    fprintf(log_file, "Error: %s\n", message);
    //    fclose(log_file);
    //}
    // Placeholder for future recovery mechanisms
}
