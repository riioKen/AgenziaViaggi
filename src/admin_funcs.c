#include "header.h"

void add_link(country_t *country, int start, int end, int is_plane) {
    int **dist_mat = NULL,
        **cost_mat = NULL,
        dist = 0,
        cost = 0;

    if(is_plane) {
        dist_mat = country->city_distances_p;
        cost_mat = country->city_costs_p;
    } else {
        dist_mat = country->city_distances_t;
        cost_mat = country->city_costs_t;
    }

    clear_terminal();

    printf("Inserisci la distanza tra le due citta' (in km): ");
    scanf("%d", &dist);
    printf("\nInserisci il costo del viaggio tra le due citta': ");
    scanf("%d", &cost);

    dist_mat[start][end] = dist_mat[end][start] = dist;
    cost_mat[start][end] = cost_mat[end][start] = cost;

    clear_terminal();
    printf("+-------------------------------------------+\n");
    printf("|    Operazione effettuata con successo!    |\n");
    printf("+-------------------------------------------+\n");
    csleep(DEFAULT_SLEEP);
}

void add_manual_link(country_t *country) {
    int start = 0,
        end = 0,
        is_plane = 0;

    clear_terminal();

    printf("Vuoi aggiungere un collegamento con aereo o con treno?");
    printf("\n0. Treno");
    printf("\n1. Aereo\n> ");
    scanf("%d", &is_plane);

    clear_terminal();
    for(int i = 0; i < country->ncities; i++) {
        printf("%d. %s\n", i, country->cities_names[i]);
    }

    printf("\nInserisci i numeri corrispondenti alle due citta' da collegare separati da uno spazio.\n> ");
    scanf("%d %d", &start, &end);

    add_link(country, start, end, is_plane);
}

/**
 *  Pannello di controllo admin
 */
void admin_control_panel(country_t *country) {
    int flag = 0,
        reports_amount = 0;
    int **reports_matrix = NULL;

    do {
        clear_terminal();

        reports_amount = fetch_reports_amount();

        if(reports_amount == -1) {
            printf("+----------------------------------------+\n");
            printf("|    Impossibile recuperare i reports    |\n");
            printf("+----------------------------------------+\n\n");
        } else if(reports_amount) {

            reports_matrix = fetch_reports(reports_amount);

            if(reports_amount == 1) {
                printf("+---------------------------+\n");
                printf("|    Hai 1 nuovo report!    |\n");
                printf("+---------------------------+\n\n");
            } else {
                printf("+----------------------------+\n");
                printf("|    Hai %-2d nuovi report!    |\n", reports_amount);
                printf("+----------------------------+\n\n");
            }
        }

        printf("Cosa si desidera fare?");
        printf("\n1. Aggiungi collegamento");
        printf("\n2. Rimuovi meta");
        printf("\n3. Controlla reports");
        printf("\n0. Esci");
        printf("\n> ");
        scanf("%d", &flag);
        getchar();

        switch(flag) {
            case 1:
                add_manual_link(country);
                update_country_db(country);
                break;
            case 2:
                clear_terminal();
                remove_city(country);
                update_country_db(country);
                csleep(DEFAULT_SLEEP);
                break;
            case 3:
                check_reports(country, reports_matrix, reports_amount);
                update_country_db(country);
                break;
            case 0:
                return;
                break;
            default:
                wrong_selection_message();
                break;
        }
    } while(flag);

    for(int i = 0; i < reports_amount; i++) {
        free(reports_matrix[i]);
    }
    free(reports_matrix);
}

void check_reports(country_t *country, int **reports, int reports_amount) {
    int choice = 0;
    for(int i = 0; i < reports_amount; i++) {
        do {
            clear_terminal();

            printf("%d -> %d ", reports[i][0], reports[i][1]);
            if(reports[i][2] == 1) {
                printf("(Aereo)\n");
            } else {
                printf("(Treno)\n");
            }

            printf("\n1. Inserisci collegamento");
            printf("\n0. Ignora report\n> ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    add_link(country, reports[i][0], reports[i][1], reports[i][2]);
                    break;
                case 0:
                    break;
                default:
                    wrong_selection_message();
                    break;
            }
        } while(choice > 1);
    }
}

int **fetch_reports(int dim) {
    FILE *reports = fopen(REPORTS_DB, "r");

    int **mat = (int **)calloc(dim, sizeof(int *));
    for(int i = 0; i < dim; i++) {
        mat[i] = (int *)calloc(3, sizeof(int));
    }

    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < 3; j++) {
            fscanf(reports, "%d", &mat[i][j]);
        }
    }

    // Riapri il database dei report in write mode per cancellarne il contenuto
    reports = freopen(REPORTS_DB, "w", reports);
    fclose(reports);
    return mat;
}

int fetch_reports_amount() {
    int reports_amount = 0;
    char buffer;
    FILE *reports = fopen(REPORTS_DB, "r");
    if(!reports) {
        return -1;
    } else {
        while(!feof(reports)) {
            buffer = fgetc(reports);
            if(buffer == '\n') {
                reports_amount++;
            }
        }

        fclose(reports);
        return reports_amount;
    }
}

/**
 *  Controllo login amministratore
 */
int login_admin() {
    int pin = 0;

    clear_terminal();
    printf("Inserisci il pin d'accesso amministratore: ");
    scanf("%d", &pin);
    getchar();

    if(pin == ADMIN_PIN) {
        return 1;
    } else {
        clear_terminal();
        printf("+------------------------+\n");
        printf("|    Password errata.    |\n");
        printf("+------------------------+\n");
        csleep(DEFAULT_SLEEP);
        return 0;
    }
}

/**
 *  Cancellazione di una città con le relative deallocazioni
 */
void remove_city(country_t *country) {
    char city_name[MAX_LONG];
    int city_index = 0,
        i = 0,
        j = 0;

    printf("Inserisci il nome della citta' da eliminare: ");
    fgets(city_name, MAX_LONG, stdin);
    city_name[strcspn(city_name, "\n")] = 0;
    city_name[0] = toupper(city_name[0]);

    if(check_city(country, city_name)) {
        city_index = get_city_index(country, city_name);
        country->city_list = remove_city_from_list(country->city_list, city_index);

        country->ncities -= 1;

        // Deallocazione del vettore relativo alla città da eliminare
        free(country->city_costs_t[city_index]);
        free(country->city_distances_t[city_index]);
        free(country->city_costs_p[city_index]);
        free(country->city_distances_p[city_index]);
        free(country->cities_names[city_index]);

        // Shift nel vettore "principale"
        for(i = city_index; i < country->ncities; i++) {
            country->city_costs_t[i] = country->city_costs_t[i + 1];
            country->city_distances_t[i] = country->city_distances_t[i + 1];
            country->city_costs_p[i] = country->city_costs_p[i + 1];
            country->city_distances_p[i] = country->city_distances_p[i + 1];
            country->cities_names[i] = country->cities_names[i + 1];
        }

        // Reallocazione dei vettori "principali"
        country->city_costs_t = (int **)realloc(country->city_costs_t, country->ncities);
        country->city_distances_t = (int **)realloc(country->city_distances_t, country->ncities);
        country->city_costs_p = (int **)realloc(country->city_costs_p, country->ncities);
        country->city_distances_p = (int **)realloc(country->city_distances_p, country->ncities);
        country->cities_names = (char **)realloc(country->cities_names, country->ncities);

        // Shift e reallocazione dei vettori di riga
        for(j = 0; j < country->ncities; j++) {
            for(i = city_index; i < country->ncities; i++) {
                country->city_costs_t[j][i] = country->city_costs_t[j][i + 1];
                country->city_distances_t[j][i] = country->city_distances_t[j][i + 1];
                country->city_costs_p[j][i] = country->city_costs_p[j][i + 1];
                country->city_distances_p[j][i] = country->city_distances_p[j][i + 1];
            }
            country->city_costs_t[j] = (int *)realloc(country->city_costs_t[j], country->ncities);
            country->city_distances_t[j] = (int *)realloc(country->city_distances_t[j], country->ncities);
            country->city_costs_p[j] = (int *)realloc(country->city_costs_p[j], country->ncities);
            country->city_distances_p[j] = (int *)realloc(country->city_distances_p[j], country->ncities);
        }

        clear_terminal();
        printf("+-----------------------------------+\n");
        printf("|    Città rimossa con successo!    |\n");
        printf("+-----------------------------------+\n");
    } else {
        clear_terminal();
        printf("+---------------------------------------+\n");
        printf("|    La citta' richiesta non esiste.    |\n");
        printf("+---------------------------------------+\n");
        csleep(DEFAULT_SLEEP);
    }
}
