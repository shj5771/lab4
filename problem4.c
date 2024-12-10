#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 5
#define MESSAGE_SIZE 100

// 공유 데이터
char message[MESSAGE_SIZE]; // 클라이언트에서 전송된 메시지
int has_message = 0;        // 메시지가 있는지 여부 (0: 없음, 1: 있음)

// 동기화를 위한 mutex와 condition 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_ready = PTHREAD_COND_INITIALIZER;

// 서버(부모) 쓰레드 함수
void* server_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 없으면 대기
        while (!has_message) {
            pthread_cond_wait(&message_ready, &mutex);
        }

        // 메시지를 모든 클라이언트에 방송
        printf("Server: Broadcasting message to all clients: %s\n", message);

        // 메시지 상태 초기화
        has_message = 0;

        pthread_mutex_unlock(&mutex);

        // 임의의 지연 (테스트용)
        sleep(1);
    }
    return NULL;
}

// 클라이언트(자식) 쓰레드 함수
void* client_thread(void* arg) {
    int client_id = *((int*)arg);
    char client_message[MESSAGE_SIZE];

    while (1) {
        // 메시지 생성
        snprintf(client_message, MESSAGE_SIZE, "Message from client %d", client_id);

        pthread_mutex_lock(&mutex);

        // 메시지 전송 요청
        while (has_message) { // 메시지가 처리되기를 기다림
            pthread_cond_wait(&message_ready, &mutex);
        }

        // 메시지 저장
        strncpy(message, client_message, MESSAGE_SIZE);
        has_message = 1;

        printf("Client %d: Sent message: %s\n", client_id, client_message);

        // 서버에게 알림
        pthread_cond_signal(&message_ready);
        pthread_mutex_unlock(&mutex);

        // 임의의 지연 (테스트용)
        sleep(rand() % 3 + 1);
    }
    return NULL;
}

int main() {
    pthread_t server;
    pthread_t clients[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }

    // 쓰레드 종료 대기 (프로그램은 무한 루프이므로 종료되지 않음)
    pthread_join(server, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    return 0;
}
