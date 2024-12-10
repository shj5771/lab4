#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

#define BUFFER_SIZE 5
#define PRODUCERS 2
#define CONSUMERS 2

int buffer[BUFFER_SIZE]; // 제한 버퍼
int count = 0;           // 버퍼의 현재 항목 수
int in = 0;              // 생산자가 데이터를 추가할 위치
int out = 0;             // 소비자가 데이터를 가져갈 위치

// 동기화를 위한 mutex와 condition 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER; // 버퍼가 비어있지 않음을 알림
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;  // 버퍼가 가득 차 있지 않음을 알림

// 생산자 쓰레드 함수
void* producer(void* arg) {
    int id = *((int*)arg);
    while (1) {
        // 생산할 데이터
        int item = rand() % 100;

        // 버퍼 접근 동기화
        pthread_mutex_lock(&mutex);

        // 버퍼가 가득 찼을 경우 대기
        while (count == BUFFER_SIZE) {
            printf("Producer %d: Buffer is full, waiting...\n", id);
            pthread_cond_wait(&not_full, &mutex);
        }

        // 버퍼에 데이터 추가
        buffer[in] = item;
        printf("Producer %d: Produced item %d at position %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        count++;

        // 소비자에게 알림
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        // 임의의 지연 (테스트용)
        sleep(rand() % 2 + 1);
    }
    return NULL;
}

// 소비자 쓰레드 함수
void* consumer(void* arg) {
    int id = *((int*)arg);
    while (1) {
        // 버퍼 접근 동기화
        pthread_mutex_lock(&mutex);

        // 버퍼가 비어있을 경우 대기
        while (count == 0) {
            printf("Consumer %d: Buffer is empty, waiting...\n", id);
            pthread_cond_wait(&not_empty, &mutex);
        }

        // 버퍼에서 데이터 가져오기
        int item = buffer[out];
        printf("Consumer %d: Consumed item %d from position %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        // 생산자에게 알림
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        // 임의의 지연 (테스트용)
        sleep(rand() % 2 + 1);
    }
    return NULL;
}

int main() {
    pthread_t producer_threads[PRODUCERS];
    pthread_t consumer_threads[CONSUMERS];
    int producer_ids[PRODUCERS];
    int consumer_ids[CONSUMERS];

    // 생산자 쓰레드 생성
    for (int i = 0; i < PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_ids[i]);
    }

    // 쓰레드 종료 대기 (프로그램은 무한 루프이므로 종료되지 않음)
    for (int i = 0; i < PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    return 0;
}
