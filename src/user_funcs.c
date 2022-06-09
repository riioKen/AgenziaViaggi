#include "header.h"

// Aggiunge il nuovo utente (nodo) alla lista
// Inserimento in testa
user_t *add_user_to_list(user_t *users_list, user_t *new) {
    if(users_list) {
        new->next = users_list;
    }
    return new;
}

void balance_management(user_t *user) {
    int choice = 0;
    do {
        clear_terminal();

        printf("Il saldo dell'account ammonta a €%.2f\n", user->balance);
        printf("\nCosa si desidera fare?");
        printf("\n1. Ricarica saldo account");
        printf("\n0. Torna indietro");
        printf("\n> ");

        scanf(" %d", &choice);
        getchar();

        switch(choice) {
            case 1:
                balance_top_up(user);
                break;
            case 0:
                break;
            default:
                wrong_selection_message();
                break;
        }
    } while(choice);
}

void balance_top_up(user_t *user) {
    float top_up = 0;
    int flag = 0;

    do {
        clear_terminal();

        printf("Inserire l'importo da ricaricare");
        printf("\n> ");
        scanf("%f", &top_up);
        getchar();

        if(top_up < 0) {
            clear_terminal();

            printf("+---------------------------------------------------------+\n");
            printf("|    Impossibile ricaricare di una quantita' negativa.    |\n");
            printf("+---------------------------------------------------------+\n");

            flag = 1;
            csleep(DEFAULT_SLEEP);
        } else if(top_up > __FLT_MAX__) {
            user->balance = __FLT_MAX__;
            clear_terminal();

            printf("+-------------------------------------------+\n");
            printf("|    Operazione effettuata con successo.    |\n");
            printf("+-------------------------------------------+\n");

            flag = 0;
            csleep(DEFAULT_SLEEP);
        } else {
            user->balance += top_up;
            clear_terminal();

            printf("+-------------------------------------------+\n");
            printf("|    Operazione effettuata con successo.    |\n");
            printf("+-------------------------------------------+\n");

            flag = 0;
            csleep(DEFAULT_SLEEP);
        }
    } while(flag);
}

// Controlla che l'email inserita appartenga ad un utente registrato
int check_email(user_t *user_list, char email[]) {
    if(user_list) {
        while(user_list) {
            if(strcmp(user_list->email, email) == 0) {
                return 1;
            }
            user_list = user_list->next;
        }
    }
    return 0;
}

// Controlla che, per una data email, la password sia corretta
int check_password(user_t *user_list, char email[], char password[]) {
    while(user_list) {
        if(strcmp(user_list->email, email) == 0) {
            if(strcmp(user_list->password, password) == 0) {
                return 1;
            }
        }
        user_list = user_list->next;
    }
    return 0;
}

// Fetch della lista di utenti dal file/database
user_t *fetch_users() {
    char first_name[MAX_LONG],
        last_name[MAX_LONG],
        email[MAX_LONG],
        password[MAX_LONG],
        *token,
        line[LINE_MAX];
    float balance;
    FILE *users = NULL;
    user_t *user_list = NULL,
        *new = NULL;

    users = fopen(USER_DB, "r");
    if(!users) {
        printf("Impossibile aprire il database utenti.\n");
        exit(1);
    } else {
        while(fgets(line, LINE_MAX, users) != NULL) {

            token = line;
            token = strtok(token, ",");
            strcpy(first_name, token);

            token = NULL;
            token = strtok(token, ",");
            strcpy(last_name, token);

            token = NULL;
            token = strtok(token, ",");
            strcpy(email, token);

            token = NULL;
            token = strtok(token, ",");
            strcpy(password, token);

            token = NULL;
            token = strtok(token, ",");
            balance = atof(token);

            // printf("%s %s %s %s\n", first_name, last_name, email, password);

            new = new_user(first_name, last_name, email, password, balance);
            user_list = add_user_to_list(user_list, new);
        }

        fclose(users);
    }
    return user_list;
}

// Deallocazione della lista di utenti
void free_user_list(user_t *user_list) {
    user_t *tmp = NULL;

    while(user_list) {
        tmp = user_list;
        user_list = user_list->next;
        free(tmp);
    }
}

// Confermate email e password viene restituito un puntatore al nodo corrispondente
user_t *get_user(user_t *user_list, char email[]) {
    if(user_list) {
        while(user_list) {
            if(strcmp(user_list->email, email) == 0) {
                return user_list;
            }
            user_list = user_list->next;
        }
    }
    return NULL;
}

// Fase di login
user_t *sign_in(user_t *user_list) {
    char email[MAX_LONG],
        password[MAX_LONG];
    user_t *user = NULL;

    clear_terminal();

    printf("Inserisci email: ");
    scanf("%s", email);
    printf("Inserisci password: ");
    scanf("%s", password);
    getchar();

    if(check_email(user_list, email)) {
        if(check_password(user_list, email, password)) {
            user = get_user(user_list, email);
        } else {
            clear_terminal();
            printf("+------------------------+\n");
            printf("|    Password errata.    |\n");
            printf("+------------------------+\n");
            csleep(3);
        }
    } else {
        clear_terminal();
        printf("+------------------------------------------+\n");
        printf("|    La email inserita non e' corretta.    |\n");
        printf("+------------------------------------------+\n");
        csleep(3);
    }

    return user;
}

// Fase di registrazione
user_t *sign_up(user_t *user_list) {
    char first_name[MAX_LONG],
        last_name[MAX_LONG],
        email[MAX_LONG],
        password[MAX_LONG];
    user_t *new = NULL;
    int flag = 0;

    clear_terminal();

    printf("Inserisci nome: ");
    fgets(first_name, MAX_LONG, stdin);
    first_name[strcspn(first_name, "\n")] = 0;
    first_name[0] = toupper(first_name[0]);

    printf("Inserisci cognome: ");
    fgets(last_name, MAX_LONG, stdin);
    last_name[strcspn(last_name, "\n")] = 0;
    last_name[0] = toupper(last_name[0]);

    do {
        printf("Inserisci email: ");
        fgets(email, MAX_LONG, stdin);
        email[strcspn(email, "\n")] = 0;

        flag = check_email(user_list, email);
        if(flag) {
            printf("\nL'email inserita e' gia' in uso.\n");
        }
    } while(flag);


    printf("Inserisci password: ");
    fgets(password, MAX_LONG, stdin);
    password[strcspn(password, "\n")] = 0;

    new = new_user(first_name, last_name, email, password, 0);
    user_list = add_user_to_list(user_list, new);

    update_user_list(user_list);

    clear_terminal();
    printf("+--------------------------------------------+\n");
    printf("|    Registrazione avvenuta con successo!    |\n");
    printf("+--------------------------------------------+\n");
    csleep(DEFAULT_SLEEP);

    return user_list;
}

// Creazione di un nuovo utente
user_t *new_user(char first_name[], char last_name[], char email[], char password[], float balance) {
    user_t *new = (user_t *)malloc(sizeof(user_t));
    if(!new) {
        fputs("ERRORE: impossibile allocare memoria.", stdout);
        csleep(DEFAULT_SLEEP);
        return NULL;
    } else {
        strcpy(new->first_name, first_name);
        strcpy(new->last_name, last_name);
        strcpy(new->email, email);
        strcpy(new->password, password);
        new->balance = balance;
        new->next = NULL;
        return new;
    }
}

// Aggiornamento della lista di utenti nel database dopo una nuova registrazione
void update_user_list(user_t *user_list) {
    FILE *users = fopen(USER_DB, "w");
    if(!users) {
        fputs("Impossibile aggiornare il database utenti.", stdout);
        csleep(DEFAULT_SLEEP);
    } else {
        while(user_list) {
            fprintf(users, "%s,%s,%s,%s,%.2f\n", user_list->first_name, user_list->last_name, user_list->email, user_list->password, user_list->balance);
            user_list = user_list->next;
        }
        fclose(users);
    }
}

/**
 *  Menu utente
 */
void user_control_panel(user_t *user, country_t *country) {
    int choice = 0;

    do {
        clear_terminal();

        printf("Ciao, %s %s!\n", user->first_name, user->last_name);

        printf("\nCosa si desidera fare?");
        printf("\n1. Prenota un viaggio");
        printf("\n2. Controlla saldo e ricarica account");
        printf("\n0. Logout");
        printf("\n> ");

        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1:
                booking_main(user, country);
                break;
            case 2:
                balance_management(user);
                break;
            case 0:
                break;
            default:
                wrong_selection_message();
                break;
        }
    } while(choice);
}
