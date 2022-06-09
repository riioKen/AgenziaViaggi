#include "header.h"

/**
 *  Funzione principale del meccanismo di prenotazione
 */
void booking_main(user_t *user, country_t *country) {
    int flag = 0,
        city_index = 0;
    do {
        switch(transport_choice()) {
            case 1:
                city_index = travel_by_plane(user, country);
                if(city_index != -1) {
                    book_hotel(country, city_index, 1);
                }
                break;
            case 2:
                city_index = travel_by_train(user, country);
                if(city_index != -1) {
                    book_hotel(country, city_index, 0);
                }
                break;
            case 0:
                break;
            default:
                wrong_selection_message();
                break;
        }
    } while(flag > 2);

}

/**
 *  Funzione di scelta del mezzo di trasporto
 */
void book_hotel(country_t *country, int city_index, int is_plane) {
    city_t *city_ptr = NULL;
    int choice = 0,
        i = 0;

    city_ptr = country->city_list;
    do {
        clear_terminal();
        printf("Scegli l'hotel da prenotare");
        city_ptr = country->city_list;
        for(i = 0; i < city_index; i++) {
            city_ptr = city_ptr->next;
        }
        for(i = 2; i < city_ptr->npoi; i++) {
            printf("\n%d. %s", i - 1, city_ptr->poi_names[i]);
        }
        printf("\n> ");
        scanf("%d", &choice);

        choice += 1;
        if(choice >= city_ptr->npoi || choice < 0) {
            wrong_selection_message();
        }
    } while(choice >= city_ptr->npoi || choice < 0);

    clear_terminal();
    if(is_plane) {
        dijkstra_city(city_ptr, 0, choice);
    } else {
        dijkstra_city(city_ptr, 1, choice);
    }
    print_divider();

    printf("\nPremere invio per terminare...");
    getchar();
    getchar();
}

/**
 *  Controllo se un nodo città ha almeno un arco entrante
 */
int check_link(int **distance_matrix, int dim, int city_index) {
    for(int i = 0; i < dim; i++) {
        if(distance_matrix[i][city_index]) {
            return 1;
        }
    }
    return 0;
}

/**
 * Funzione di Dijkstra adattata alla ricerca del percorso minimo in città
 */
void dijkstra_city(city_t *city, int start, int end) {
    int *distance = (int *)calloc(city->npoi, sizeof(int)),
        *previous = (int *)calloc(city->npoi, sizeof(int)),
        i = 0,
        j = 0,
        u = 0,
        hr = 0,
        min = 0;
    bool *done = (bool *)calloc(city->npoi, sizeof(bool));
    float dist = 0;

    for(i = 0; i < city->npoi; i++) {
        distance[i] = __INT_MAX__;
        previous[i] = -1;
    }

    distance[start] = 0;

    for(i = 0; i < city->npoi - 1; i++) {
        // Trova il vertice con distanza minima tra i vertici che non sono stati ancora processati
        // Nella prima iterazione u è sempre uguale a start
        u = get_minimum_distance(city->npoi, distance, done);
        // Segna il vertice trovato come processato
        done[u] = true;
        // Aggiorna i valori delle distanze per i vertici adiacenti a quello trovato
        for(j = 0; j < city->npoi; j++) {
            // Aggiorna la distanza con j solo se questo non è stato processato,
            // se c'è un arco tra u e j e se il peso totale del percorso da start
            // a j passando per u è più piccolo del valore attuale di distance[j]
            if(done[j] == false && city->poi[u][j] && distance[u] + city->poi[u][j] < distance[j]) {
                previous[j] = u;
                distance[j] = distance[u] + city->poi[u][j];
            }
        }
    }

    i = end;
    do {
        dist += city->poi[i][previous[i]] * 1.5;
        i = previous[i];
    } while(previous[i] != -1);

    if(dist > 60) {
        hr = dist / 60;
        min = (int)dist % 60;
    } else {
        min = dist;
    }

    print_shortest_path(city->poi_names, distance, previous, start, end);

    printf("Durata del viaggio: ");

    if(hr != 0) {
        if(hr > 1) {
            printf("%d ore e ", hr);
        } else if(hr == 1) {
            printf("%d ora e ", hr);
        }
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    } else {
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    }

    free(distance);
    free(previous);
    free(done);
}

/**
 *  Funzione di Dijkstra per la ricerca del viaggio più economico
 */
float dijkstra_cost(country_t *country, int **cost_matrix, int **distance_matrix, int start, int end) {
    int *distance = (int *)calloc(country->ncities, sizeof(int)),
        *previous = (int *)calloc(country->ncities, sizeof(int)),
        i = 0,
        j = 0,
        u = 0,
        hr = 0,
        min = 0;
    float cost = 0,
        dist = 0;
    bool *done = (bool *)calloc(country->ncities, sizeof(bool));

    // Inizializziamo le distanze ad infinito
    // e i nodi precedenti a -1
    for(i = 0; i < country->ncities; i++) {
        distance[i] = __INT_MAX__;
        previous[i] = -1;
    }

    // La distanza del nodo di partenza da sè stesso è 0
    distance[start] = 0;

    // Ricerca del percorso minimo
    for(i = 0; i < country->ncities - 1; i++) {
        // Trova il vertice con distanza minima tra i vertici che non sono stati ancora processati
        // Nella prima iterazione u è sempre uguale a start
        u = get_minimum_distance(country->ncities, distance, done);
        // Segna il vertice trovato come processato
        done[u] = true;
        // Aggiorna i valori delle distanze per i vertici adiacenti a quello trovato
        for(j = 0; j < country->ncities; j++) {
            // Aggiorna la distanza con j solo se questo non è stato processato,
            // se c'è un arco tra u e j e se il peso totale del percorso da start
            // a j passando per u è più piccolo del valore attuale di distance[j]
            if(done[j] == false && cost_matrix[u][j] && distance[u] + cost_matrix[u][j] < distance[j]) {
                previous[j] = u;
                distance[j] = distance[u] + cost_matrix[u][j];
            }
        }
    }

    // Calcolo del costo e del tempo di viaggio
    i = end;
    do {
        cost += cost_matrix[i][previous[i]];
        dist += distance_matrix[i][previous[i]] * 0.1;
        i = previous[i];
    } while(previous[i] != -1);

    // Quantificazione del tempo di viaggio in ore e minuti
    if(dist > 60) {
        hr = dist / 60;
        min = (int)dist % 60;
    } else {
        min = dist;
    }


    /**
     *  Stampa delle informazioni
     */
    print_shortest_path(country->cities_names, distance, previous, start, end);
    printf("Costo del viaggio: €%.2f", cost);
    printf("\nDurata del viaggio: ");

    if(hr != 0) {
        if(hr > 1) {
            printf("%d ore e ", hr);
        } else if(hr == 1) {
            printf("%d ora e ", hr);
        }
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    } else {
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    }

    free(distance);
    free(previous);
    free(done);

    return cost;
}

/**
 *  Funzione di Dijkstra per la ricerca del viaggio più veloce
 */
float dijkstra_distance(country_t *country, int **cost_matrix, int **distance_matrix, int start, int end) {
    int *distance = (int *)calloc(country->ncities, sizeof(int)),
        *previous = (int *)calloc(country->ncities, sizeof(int)),
        i = 0,
        j = 0,
        u = 0,
        min = 0,
        hr = 0;
    float cost = 0,
        dist = 0;
    bool *done = (bool *)calloc(country->ncities, sizeof(bool));

    // Inizializziamo le distanze ad infinito
    // e i nodi precedenti a -1
    for(i = 0; i < country->ncities; i++) {
        distance[i] = __INT_MAX__;
        previous[i] = -1;
    }

    // La distanza del nodo di partenza da sè stesso è 0
    distance[start] = 0;

    // Ricerca del percorso minimo
    for(i = 0; i < country->ncities - 1; i++) {
        // Trova il vertice con distanza minima tra i vertici che non sono stati ancora processati
        // Nella prima iterazione u è sempre uguale a start
        u = get_minimum_distance(country->ncities, distance, done);
        // Segna il vertice trovato come processato
        done[u] = true;
        // Aggiorna i valori delle distanze per i vertici adiacenti a quello trovato
        for(j = 0; j < country->ncities; j++) {
            // Aggiorna la distanza con j solo se questo non è stato processato,
            // se c'è un arco tra u e j e se il peso totale del percorso da start
            // a j passando per u è più piccolo del valore attuale di distance[j]
            if(done[j] == false && distance_matrix[u][j] && distance[u] + distance_matrix[u][j] < distance[j]) {
                previous[j] = u;
                distance[j] = distance[u] + distance_matrix[u][j];
            }
        }
    }

    i = end;
    do {
        dist += distance_matrix[i][previous[i]] * 0.1;
        cost += cost_matrix[i][previous[i]];
        i = previous[i];
    } while(previous[i] != -1);

    if(dist > 60) {
        hr = dist / 60;
        min = (int)dist % 60;
    } else {
        min = dist;
    }

    print_shortest_path(country->cities_names, distance, previous, start, end);
    printf("Costo del viaggio: €%.2f", cost);
    printf("\nDurata del viaggio: ");

    if(hr != 0) {
        if(hr > 1) {
            printf("%d ore e ", hr);
        } else if(hr == 1) {
            printf("%d ora e ", hr);
        }
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    } else {
        if(min > 1) {
            printf("%d minuti.\n", min);
        } else if(min == 1) {
            printf("%d minuto.\n", min);
        }
    }

    free(distance);
    free(previous);
    free(done);

    return cost;
}

int get_minimum_distance(int dim, int distance[], bool done[]) {
    int min = __INT_MAX__,
        min_index = 0;

    for(int i = 0; i < dim; i++) {
        if(done[i] == false && distance[i] <= min) {
            min = distance[i];
            min_index = i;
        }
    }
    return min_index;
}

void print_shortest_path(char **names, int distance[], int previous[], int start, int end) {
    printf("\n%s -> ", names[start]);
    print_path(names, previous, end);
    printf("|\n");
}

void print_path(char **names, int previous[], int index) {
    if(previous[index] == -1) {
        return;
    } else {
        print_path(names, previous, previous[index]);
        printf("%s -> ", names[index]);
    }
}

void report_missing_link(int start, int end, int is_plane) {
    FILE *reports = fopen(REPORTS_DB, "a");
    if(!reports) {
        clear_terminal();
        printf("+-----------------------------------------------+\n");
        printf("|    Impossibile effettuare la segnalazione.    |\n");
        printf("+-----------------------------------------------+\n");
        csleep(DEFAULT_SLEEP);
    } else {
        fprintf(reports, "%d %d %d\n", start, end, is_plane);

        clear_terminal();
        printf("+---------------------------------------------+\n");
        printf("|    Segnalazione effettuata con successo!    |\n");
        printf("+---------------------------------------------+\n");
        csleep(DEFAULT_SLEEP);

        fclose(reports);
    }
}

int transport_choice() {
    int choice = 0;

    do {
        clear_terminal();

        printf("Con quale mezzo desideri viaggiare?");
        printf("\n1. Aereo");
        printf("\n2. Treno");
        printf("\n0. Esci");
        printf("\n> ");

        scanf("%d", &choice);
        getchar();

        if(choice < 0 || choice > 2) {
            wrong_selection_message();
        }
    } while(choice < 0 || choice > 2);

    return choice;
}

int travel_by_plane(user_t *user, country_t *country) {
    int start = 0,
        end = 0,
        choice = 0,
        i = 0;
    float cost_fast = 0,
        cost_cheap = 0;
    char confirm = 0;

    clear_terminal();

    printf("Scegli la citta' di partenza:");
    for(i = 0; i < country->ncities; i++) {
        printf("\n%d. %s", i, country->cities_names[i]);
    }
    printf("\n> ");
    scanf("%d", &start);

    printf("\nSeleziona la citta' di arrivo:");
    for(i = 0; i < country->ncities; i++) {
        printf("\n%d. %s", i, country->cities_names[i]);
    }
    printf("\n> ");
    scanf("%d", &end);

    if(check_link(country->city_distances_p, country->ncities, end) == 0 || check_link(country->city_distances_p, country->ncities, start) == 0) {
        clear_terminal();

        printf("+-------------------------------------------------------------------+\n");
        printf("|    La citta' di destinazione selezionata non e' raggiungibile.    |\n");
        printf("|        Vorreste segnalare il problema agli amministratori?        |\n");
        printf("+-------------------------------------------------------------------+\n");

        printf("\n(y/n)> ");
        getchar();
        scanf("%c", &confirm);

        switch(confirm) {
            case 'y':
                report_missing_link(start, end, 1);
                break;
            case 'n':
                break;
        }
        return -1;
    } else if(start == end) {
        clear_terminal();
        printf("+---------------------------------+\n");
        printf("|    Seleziona citta' diverse.    |\n");
        printf("+---------------------------------+\n");
        csleep(DEFAULT_SLEEP);

        return -1;
    } else {
        printf("\n");
        print_divider();
        printf("Viaggio piu' veloce:");
        cost_fast = dijkstra_distance(country, country->city_costs_p, country->city_distances_p, start, end);

        printf("\nViaggio piu' economico:");
        cost_cheap = dijkstra_cost(country, country->city_costs_p, country->city_distances_p, start, end);
        print_divider();

        do {
            printf("\nQuale viaggio desideri prenotare?");
            printf("\n1. Viaggio piu' veloce");
            printf("\n2. Viaggio piu' economico");
            printf("\n0. Annulla e torna indietro");
            printf("\n> ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    if(user->balance < cost_fast) {
                        clear_terminal();
                        printf("Il tuo saldo non e' sufficiente, vuoi ricaricare? (y/n): ");
                        getchar();
                        scanf("%c", &confirm);
                        if(confirm == 'y') {
                            balance_top_up(user);

                            user->balance -= cost_fast;

                            clear_terminal();
                            printf("+-----------------------------------------+\n");
                            printf("|    Acquisto completato con successo!    |\n");
                            printf("+-----------------------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        } else {
                            clear_terminal();
                            printf("+-----------------------------+\n");
                            printf("|    Operazione annullata.    |\n");
                            printf("+-----------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        }
                    } else {

                        user->balance -= cost_fast;
                        clear_terminal();
                        printf("+-----------------------------------------+\n");
                        printf("|    Acquisto completato con successo!    |\n");
                        printf("+-----------------------------------------+\n");
                        csleep(DEFAULT_SLEEP);
                    }
                    break;
                case 2:
                    if(user->balance < cost_cheap) {
                        clear_terminal();
                        printf("Il tuo saldo non e' sufficiente, vuoi ricaricare? (y/n): ");
                        getchar();
                        scanf("%c", &confirm);
                        if(confirm == 'y') {
                            balance_top_up(user);

                            user->balance -= cost_cheap;

                            clear_terminal();
                            printf("+-----------------------------------------+\n");
                            printf("|    Acquisto completato con successo!    |\n");
                            printf("+-----------------------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        } else {
                            clear_terminal();
                            printf("+-----------------------------+\n");
                            printf("|    Operazione annullata.    |\n");
                            printf("+-----------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        }
                    } else {

                        user->balance -= cost_cheap;
                        clear_terminal();
                        printf("+-----------------------------------------+\n");
                        printf("|    Acquisto completato con successo!    |\n");
                        printf("+-----------------------------------------+\n");
                        csleep(DEFAULT_SLEEP);
                    }
                    break;
                case 0:
                    return -1;
                    break;
                default:
                    clear_terminal();
                    wrong_selection_message();
                    csleep(DEFAULT_SLEEP);
                    break;
            }
        } while(choice > 2);

        return end;
    }
}

int travel_by_train(user_t *user, country_t *country) {
    int start = 0,
        end = 0,
        choice = 0,
        i = 0;
    float cost_fast = 0,
        cost_cheap = 0;
    char confirm = 0;

    clear_terminal();

    printf("Scegli la citta' di partenza:");
    for(i = 0; i < country->ncities; i++) {
        printf("\n%d. %s", i, country->cities_names[i]);
    }
    printf("\n> ");
    scanf("%d", &start);

    printf("\nSeleziona la citta' di arrivo:");
    for(i = 0; i < country->ncities; i++) {
        printf("\n%d. %s", i, country->cities_names[i]);
    }
    printf("\n> ");
    scanf("%d", &end);

    if(check_link(country->city_distances_t, country->ncities, end) == 0 || check_link(country->city_distances_t, country->ncities, start) == 0) {
        clear_terminal();

        printf("+-------------------------------------------------------------------+\n");
        printf("|    La citta' di destinazione selezionata non e' raggiungibile.    |\n");
        printf("|        Vorreste segnalare il problema agli amministratori?        |\n");
        printf("+-------------------------------------------------------------------+\n");

        printf("\n(y/n)> ");
        getchar();
        scanf("%c", &confirm);

        switch(confirm) {
            case 'y':
                report_missing_link(start, end, 0);
                break;
            case 'n':
                break;
        }

        return -1;
    } else if(start == end) {
        clear_terminal();
        printf("+---------------------------------+\n");
        printf("|    Seleziona citta' diverse.    |\n");
        printf("+---------------------------------+\n");
        csleep(DEFAULT_SLEEP);

        return -1;
    } else {
        printf("\n");
        print_divider();
        printf("Viaggio piu' veloce:");
        cost_fast = dijkstra_distance(country, country->city_costs_t, country->city_distances_t, start, end);

        printf("\nViaggio piu' economico:");
        cost_cheap = dijkstra_cost(country, country->city_costs_t, country->city_distances_t, start, end);
        print_divider();

        do {
            printf("\nQuale viaggio desideri prenotare?");
            printf("\n1. Viaggio piu' veloce");
            printf("\n2. Viaggio piu' economico");
            printf("\n0. Annulla e torna indietro");
            printf("\n> ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    if(user->balance < cost_fast) {
                        clear_terminal();
                        printf("Il tuo saldo non e' sufficiente, vuoi ricaricare? (y/n): ");
                        getchar();
                        scanf("%c", &confirm);
                        if(confirm == 'y') {
                            balance_top_up(user);

                            user->balance -= cost_fast;

                            clear_terminal();
                            printf("+-----------------------------------------+\n");
                            printf("|    Acquisto completato con successo!    |\n");
                            printf("+-----------------------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        } else {
                            clear_terminal();
                            printf("+-----------------------------+\n");
                            printf("|    Operazione annullata.    |\n");
                            printf("+-----------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        }
                    } else {

                        user->balance -= cost_fast;
                        clear_terminal();
                        printf("+-----------------------------------------+\n");
                        printf("|    Acquisto completato con successo!    |\n");
                        printf("+-----------------------------------------+\n");
                        csleep(DEFAULT_SLEEP);
                    }
                    break;
                case 2:
                    if(user->balance < cost_cheap) {
                        clear_terminal();
                        printf("Il tuo saldo non e' sufficiente, vuoi ricaricare? (y/n): ");
                        getchar();
                        scanf("%c", &confirm);
                        if(confirm == 'y') {
                            balance_top_up(user);

                            user->balance -= cost_cheap;

                            clear_terminal();
                            printf("+-----------------------------------------+\n");
                            printf("|    Acquisto completato con successo!    |\n");
                            printf("+-----------------------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        } else {
                            clear_terminal();
                            printf("+-----------------------------+\n");
                            printf("|    Operazione annullata.    |\n");
                            printf("+-----------------------------+\n");
                            csleep(DEFAULT_SLEEP);
                        }
                    } else {

                        user->balance -= cost_cheap;
                        clear_terminal();
                        printf("+-----------------------------------------+\n");
                        printf("|    Acquisto completato con successo!    |\n");
                        printf("+-----------------------------------------+\n");
                        csleep(DEFAULT_SLEEP);
                    }
                    break;
                case 0:
                    return -1;
                    break;
                default:
                    clear_terminal();
                    wrong_selection_message();
                    csleep(DEFAULT_SLEEP);
                    break;
            }
        } while(choice > 2);

        return end;
    }
}
