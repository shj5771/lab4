#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// 쓰레드에서 실행될 함수 정의
void* thread_function(void* arg) {
    int thread_id = *((int*)arg);
    printf("Thread %d: Hello from thread!\n", thread_id);

    // 쓰레드가 수행하는 작업
    for (int i = 0; i < 5; i++) {
        printf("Thread %d: Counting %d\n", thread_id, i + 1);
    }

    printf("Thread %d: Goodbye from thread!\n", thread_id);
    return NULL; // 쓰레드 종료
}

int main() {
    const int NUM_THREADS = 3; // 생성할 쓰레드 개수
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code;

    // 쓰레드 생성
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i + 1; // 쓰레드 ID
        printf("Main: creating thread %d\n", i + 1);
        result_code = pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);

        if (result_code) { // 에러 처리
            fprintf(stderr, "Error - pthread_create() return code: %d\n", result_code);
            exit(EXIT_FAILURE);
        }
    }

    // 모든 쓰레드 종료 대기
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Main: thread %d has finished\n", i + 1);
    }

    printf("Main: All threads are done. Exiting program.\n");
    return 0;
}
