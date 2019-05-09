/* User  */
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>

#include "banking_aux.h"
#include "types.h"
#include "sope.h"
#include "constants.h"
#include "general_aux.h"


int main(int argc, char *argv[]){

    /** Verify command **/
    if (argc < 6){
        printf("user [Account ID] [Account Password] [Delay in ms] [Operation code] [Argument list]\n");
        exit(-1);
    }
    if (!is_valid_password(argv[2])){
        printf ("Password needs to have between %d and %d characters\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
        exit(EXIT_FAILURE);
    }
    
    /** Build request struct **/
    tlv_request_t request;
    if (!build_tlv_request(&request, argv))
        exit(EXIT_FAILURE);
    
    /** Log Request **/
    int user_log_fd = open(USER_LOGFILE, O_CREAT | O_WRONLY | O_APPEND);
    logRequest(user_log_fd, request.value.header.pid, &request);
    close(user_log_fd);
    
    /** Create user FIFO **/
    char fifo_path[USER_FIFO_PATH_LEN];
    sprintf(fifo_path, "%s%0*d",USER_FIFO_PATH_PREFIX, WIDTH_ID, request.value.header.pid);
    create_fifo(fifo_path);
    
    /** Write request to server fifo **/
    int server_fifo_fd = open(SERVER_FIFO_PATH, O_WRONLY);
    if (server_fifo_fd == -1){
        printf("Server is not available\n");
        exit(EXIT_FAILURE);
    }

    write(server_fifo_fd, &request, sizeof(request));
    close (server_fifo_fd);

    /** Wait for server response **/ 
    clock_t start_t = clock();
    while ( ((double)(clock()-start_t) / CLOCKS_PER_SEC) < FIFO_TIMEOUT_SECS){
        /**
         * TODO: Check for server response
         * TODO: Log server response
         */
    } 

    /** Remove user FIFO **/
    remove_fifo(fifo_path);

    exit(EXIT_SUCCESS);
} 
