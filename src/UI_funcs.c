#include "header.h"

/**
 *  Funzione cross-platform per pulizia del terminale
 */
void clear_terminal() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 *  Funzione cross-platform di wait
 */
void csleep(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

// Schermata di login
int main_menu() {
    int flag = 0;

    print_logo();

    printf("\n1. Login");
    printf("\n2. Registrati");
    printf("\n3. Accesso Amministratore");
    printf("\n0. Esci");
    printf("\n> ");
    scanf("%d", &flag);
    getchar();

    return flag;
}

void print_divider() {
    printf("+");
    for(int i = 0; i < MAX_LONG; i++) {
        printf("-");
    }
    printf("+\n");
}

void print_logo() {
    printf(" _______ _      _        _   ______        _   \n");
    printf("|__   __(_)    | |      | | |  ____|      | |  \n");
    printf("   | |   _  ___| | _____| |_| |__ __ _ ___| |_ \n");
    printf("   | |  | |/ __| |/ / _ \\ __|  __/ _` / __| __|\n");
    printf("   | |  | | (__|   <  __/ |_| | | (_| \\__ \\ |_ \n");
    printf("   |_|  |_|\\___|_|\\_\\___|\\__|_|  \\__,_|___/\\__|\n");
}

void wrong_selection_message() {
    clear_terminal();
    printf("+-----------------------------------+\n");
    printf("|    Inserire un'opzione valida.    |\n");
    printf("+-----------------------------------+\n");
    csleep(DEFAULT_SLEEP);
}