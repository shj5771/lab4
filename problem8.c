#include <gtk/gtk.h>

// 버튼 클릭 이벤트 핸들러
void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Hello, World!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *button;

    // GTK 초기화
    gtk_init(&argc, &argv);

    // 윈도우 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Problem 8 - GTK+ Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // 창 닫기 이벤트 처리
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 버튼 생성
    button = gtk_button_new_with_label("Click Me");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // 버튼을 창에 추가
    gtk_container_add(GTK_CONTAINER(window), button);

    // 윈도우 표시
    gtk_widget_show_all(window);

    // GTK 메인 루프 실행
    gtk_main();

    return 0;
}
