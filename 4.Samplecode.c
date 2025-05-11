#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10
#define NUM_INCREMENTS 1000000

// Shared counter
// counter는 모든 쓰레드가 공유하는 변수
int counter = 0;

// Mutex for synchronizing access to the counter
// counter에 접근하는 것을 동기화하기 위한 뮤텍스
pthread_mutex_t counter_mutex;

// Function to be executed by each thread
// 각 쓰레드가 실행할 함수
void* increment_counter(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        // Lock the mutex before accessing the counter
        // counter에 접근하기 전에 뮤텍스 잠그기
        pthread_mutex_lock(&counter_mutex);
        
        // Increment the counter
        // counter에 접근하여 1만큼 증가시킴
        counter++;
        
        // Unlock the mutex after accessing the counter
        // counter에 접근이 끝나면 뮤텍스 잠금해제
        pthread_mutex_unlock(&counter_mutex);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    
    // Initialize the mutex
    // 뮤텍스 초기화
    pthread_mutex_init(&counter_mutex, NULL);
    
    // Create threads
    // 쓰레드 생성
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_create(&threads[i], NULL, increment_counter, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    // Wait for all threads to finish
    // 쓰레드 끝날 때까지 대기
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }
    
    // Destroy the mutex
    // 뮤텍스 삭제
    pthread_mutex_destroy(&counter_mutex);
    
    // Print the final value of the counter
    // counter값 출력
    printf("Final counter value: %d\n", counter);
    
    return 0;
}
