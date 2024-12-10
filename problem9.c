#include <gtk/gtk.h>
#include <string.h>

// 계산기 상태
typedef struct {
    char current_input[256];
    double result;
    char operation;
} CalculatorState;

CalculatorState calc_state = { .current_input = "", .result = 0.0, .operation = '\0' };

// 버튼 클릭 이벤트 처리 함수
void on_button_clicked(GtkWidget *widget, gpointer data) {
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));
    char *input = calc_state.current_input;

    // 숫자 또는 "." 처리
    if (g_ascii_isdigit(button_label[0]) || strcmp(button_label, ".") == 0) {
        strcat(input, button_label);
        g_print("Input: %s\n", input);
    }
    // 연산자 처리
    else if (strchr("+-*/", button_label[0])) {
        calc_state.result = atof(input); // 현재 입력값을 숫자로 변환하여 저장
        calc_state.operation = button_label[0]; // 연산자 저장
        input[0] = '\0'; // 입력 초기화
        g_print("Operation: %c, Result: %.2f\n", calc_state.operation, calc_state.result);
    }
    // "=" 처리 (계산 수행)
    else if (strcmp(button_label, "=") == 0) {
        double second_operand = atof(input); // 두 번째 숫자
        switch (calc_state.operation) {
            case '+': calc_state.result += second_operand; break;
            case '-': calc_state.result -= second_operand; break;
            case '*': calc_state.result *= second_operand; break;
            case '/': 
                if (second_operand != 0) 
                    calc_state.result /= second_operand; 
                else 
                    g_print("Error: Division by zero!\n");
                break;
        }
        snprintf(input, sizeof(calc_state.current_input), "%.2f", calc_state.result); // 결과 저장
        g_print("Result: %.2f\n", calc_state.result);
        calc_state.operation = '\0'; // 연산자 초기화
    }
    // "C" 처리 (초기화)
    else if (strcmp(button_label, "C") == 0) {
        calc_state.result = 0.0;
        calc_state.operation = '\0';
        input[0] = '\0';
        g_print("Cleared\n");
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *button;

    gtk_init(&argc, &argv);

    // 메인 윈도우 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);


    // 그리드 레이아웃 생성
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 버튼 추가
    const char *buttons[] = { "7", "8", "9", "/", 
                              "4", "5", "6", "*", 
                              "1", "2", "3", "-", 
                              "0", "C", "=", "+" };
    int pos = 0;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            button = gtk_button_new_with_label(buttons[pos++]);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        }
    }

    // 윈도우 종료 이벤트 처리
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 위젯 표시 및 GTK 메인 루프 실행
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
