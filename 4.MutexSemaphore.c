#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

// Global variables
char target_key = 'k';              // The key to monitor
volatile bool is_key_pressed = false; // Flag indicating if the key is pressed
volatile bool exit_program = false;   // Flag to signal program termination

// Shared counter
int counter = 0;

// Mutex for protecting shared resources
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to be executed by thread handling key press
void* print_when_pressed(void* arg) {
    while (!exit_program) {
        // Check if the key is pressed
        pthread_mutex_lock(&state_mutex);
        bool key_state = is_key_pressed;
        pthread_mutex_unlock(&state_mutex);
        
        if (key_state) {
            // Key is pressed - do work
            pthread_mutex_lock(&state_mutex);
            counter++;
            printf("Key is PRESSED! First Thread running// Counter: %d\n", counter);
            pthread_mutex_unlock(&state_mutex);
            
            // Sleep to avoid flooding the output
            usleep(1500000); // 1.5 seconds
        } else {
            // Key is not pressed - wait briefly before checking again
            usleep(50000); // 50 milliseconds
        }
    }
    return NULL;
}

// Function to be executed by thread handling key release
void* print_when_released(void* arg) {
    while (!exit_program) {
        // Check if the key is released
        pthread_mutex_lock(&state_mutex);
        bool key_state = is_key_pressed;
        pthread_mutex_unlock(&state_mutex);
        
        if (!key_state) {
            // Key is released - do work
            pthread_mutex_lock(&state_mutex);
            counter++;
            printf("Key is RELEASED! Second Thread running// Counter: %d\n", counter);
            pthread_mutex_unlock(&state_mutex);
            
            // Sleep to avoid flooding the output
            usleep(1500000); // 1.5 seconds
        } else {
            // Key is pressed - wait briefly before checking again
            usleep(50000); // 50 milliseconds
        }
    }
    return NULL;
}

int main() {
    pthread_t press_thread, release_thread;
    struct termios orig_termios, new_termios;
    
    // Setup terminal for raw mode
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    
    // Set stdin to non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    
    printf("Key monitoring program started.\n");
    printf("Press 'k' to trigger the first thread.\n");
    printf("Release 'k' to trigger the second thread.\n");
    printf("Press 'q' to exit.\n\n");
    
    // Create the threads
    if (pthread_create(&press_thread, NULL, print_when_pressed, NULL) != 0) {
        perror("Failed to create press thread");
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        return 1;
    }
    
    if (pthread_create(&release_thread, NULL, print_when_released, NULL) != 0) {
        perror("Failed to create release thread");
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        return 1;
    }
    
    // Variables for key state detection
    struct timeval last_key_press_time;
    gettimeofday(&last_key_press_time, NULL);
    bool key_was_pressed = false;
    const long KEY_RELEASE_TIMEOUT_MS = 100; // 100ms timeout for key release detection
    
    // Main loop to monitor keyboard input
    while (!exit_program) {
        fd_set readfds;
        struct timeval tv;
        
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        tv.tv_sec = 0;
        tv.tv_usec = 20000; // 20ms timeout for select
        
        int select_result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        
        if (select_result == -1) {
            perror("select()");
            break;
        } else if (select_result > 0) {
            // Input is available
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                if (ch == 'q') {
                    // Exit program
                    exit_program = true;
                } else if (ch == target_key) {
                    // Target key is pressed
                    pthread_mutex_lock(&state_mutex);
                    is_key_pressed = true;
                    pthread_mutex_unlock(&state_mutex);
                    
                    // Record the time of key press
                    gettimeofday(&last_key_press_time, NULL);
                    key_was_pressed = true;
                }
            }
        } else {
            // No input available - check if we need to simulate key release
            if (key_was_pressed) {
                struct timeval current_time;
                gettimeofday(&current_time, NULL);
                
                // Calculate elapsed time in milliseconds
                long elapsed_ms = (current_time.tv_sec - last_key_press_time.tv_sec) * 1000 + 
                                 (current_time.tv_usec - last_key_press_time.tv_usec) / 1000;
                
                // If enough time has passed without seeing the key again, consider it released
                if (elapsed_ms > KEY_RELEASE_TIMEOUT_MS) {
                    pthread_mutex_lock(&state_mutex);
                    is_key_pressed = false;
                    pthread_mutex_unlock(&state_mutex);
                    key_was_pressed = false;
                }
            }
        }
    }
    
    // Wait for threads to finish
    pthread_join(press_thread, NULL);
    pthread_join(release_thread, NULL);
    
    // Cleanup
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    pthread_mutex_destroy(&state_mutex);
    
    printf("Final counter value: %d\n", counter);
    
    return 0;
}