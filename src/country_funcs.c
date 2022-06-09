#include "header.h"

/**
 *  Controllo se una città è presente nella lista
 */
int check_city(country_t *country, char city_name[]) {
    for(int i = 0; i < country->ncities; i++) {
        if(strcmp(country->cities_names[i], city_name) == 0) {
            return 1;
        }
    }
    return 0;
}

char **create_char_matrix(int row, int length) {
    char **m = (char **)calloc(row, sizeof(char *));
    for(int i = 0; i < row; i++) {
        m[i] = (char *)calloc(length, sizeof(char));
    }

    return m;
}

int **create_int_matrix(int dim) {
    int **m = (int **)calloc(dim, sizeof(int *));
    for(int i = 0; i < dim; i++) {
        m[i] = (int *)calloc(dim, sizeof(int));
    }

    return m;
}

void free_country(country_t *country) {
    free_country_matrix(country);
    free_city_list(country->city_list);
}

void free_country_matrix(country_t *country) {
    int i = 0;

    for(i = 0; i < country->ncities; i++) {
        free(country->city_costs_t[i]);
    }
    free(country->city_costs_t);

    for(i = 0; i < country->ncities; i++) {
        free(country->city_distances_t[i]);
    }
    free(country->city_distances_t);

    for(i = 0; i < country->ncities; i++) {
        free(country->city_costs_p[i]);
    }
    free(country->city_costs_p);

    for(i = 0; i < country->ncities; i++) {
        free(country->city_distances_p[i]);
    }
    free(country->city_distances_p);

    for(i = 0; i < country->ncities; i++) {
        free(country->cities_names[i]);
    }
    free(country->cities_names);

    country->city_costs_t = NULL;
    country->city_distances_t = NULL;
    country->city_costs_p = NULL;
    country->city_distances_p = NULL;
    country->cities_names = NULL;
}

country_t *init_country() {
    int i = 0,
        j = 0;
    char trash;
    FILE *city_db = fopen(CITY_DB, "r");
    if(!city_db) {
        printf("Errore: database citta' non trovato.\n");
        csleep(DEFAULT_SLEEP);
        exit(1);
    } else {
        country_t *new = new_country();
        if(!new) {
            printf("\nErrore: impossibile allocare la memoria.\n");
            csleep(DEFAULT_SLEEP);
            return NULL;
        } else {
            // Fetch del numero delle città
            fscanf(city_db, "%d", &new->ncities);

            new->cities_names = create_char_matrix(new->ncities, MAX_LONG);
            new->city_costs_t = create_int_matrix(new->ncities);
            new->city_distances_t = create_int_matrix(new->ncities);
            new->city_costs_p = create_int_matrix(new->ncities);
            new->city_distances_p = create_int_matrix(new->ncities);

            fscanf(city_db, "%c", &trash);

            // Fetch dei nomi delle città
            for(i = 0; i < new->ncities; i++) {
                fgets(new->cities_names[i], MAX_LONG, city_db);
                new->cities_names[i][strcspn(new->cities_names[i], "\n")] = 0;
            }

            for(i = 0; i < new->ncities; i++) {
                for(j = 0; j < new->ncities; j++) {
                    fscanf(city_db, "%d", &new->city_costs_t[i][j]);
                }
            }

            for(i = 0; i < new->ncities; i++) {
                for(j = 0; j < new->ncities; j++) {
                    fscanf(city_db, "%d", &new->city_distances_t[i][j]);
                }
            }

            for(i = 0; i < new->ncities; i++) {
                for(j = 0; j < new->ncities; j++) {
                    fscanf(city_db, "%d", &new->city_costs_p[i][j]);
                }
            }

            for(i = 0; i < new->ncities; i++) {
                for(j = 0; j < new->ncities; j++) {
                    fscanf(city_db, "%d", &new->city_distances_p[i][j]);
                }
            }

            new->city_list = fetch_cities(city_db, new->ncities);
            if(!new->city_list) {
                printf("Errore: raccolta dettagli citta' non riuscita.\n");
                free_country_matrix(new);
                free(new);
                csleep(DEFAULT_SLEEP);
                return NULL;
            }

            fclose(city_db);
            return new;
        }
    }
}

country_t *new_country() {
    country_t *new = (country_t *)malloc(sizeof(country_t));
    if(!new) {
        return NULL;
    } else {
        new->ncities = 0;
        new->city_costs_t = NULL;
        new->city_distances_t = NULL;
        new->city_costs_p = NULL;
        new->city_distances_p = NULL;
        new->cities_names = NULL;
        new->city_list = NULL;
        return new;
    }
}

void update_country_db(country_t *country) {
    int i = 0,
        j = 0;
    FILE *city_db = fopen(CITY_DB, "w");
    if(!city_db) {
        printf("Errore: impossibile trovare e/o aggiornare il database citta'.\n");
        csleep(DEFAULT_SLEEP);
    } else {
        fprintf(city_db, "%d\n", country->ncities);

        for(i = 0; i < country->ncities; i++) {
            fprintf(city_db, "%s\n", country->cities_names[i]);
        }

        for(i = 0; i < country->ncities; i++) {
            for(j = 0; j < country->ncities - 1; j++) {
                fprintf(city_db, "%d ", country->city_costs_t[i][j]);
            }
            fprintf(city_db, "%d\n", country->city_costs_t[i][j]);
        }

        for(i = 0; i < country->ncities; i++) {
            for(j = 0; j < country->ncities - 1; j++) {
                fprintf(city_db, "%d ", country->city_distances_t[i][j]);
            }
            fprintf(city_db, "%d\n", country->city_distances_t[i][j]);
        }

        for(i = 0; i < country->ncities; i++) {
            for(j = 0; j < country->ncities - 1; j++) {
                fprintf(city_db, "%d ", country->city_costs_p[i][j]);
            }
            fprintf(city_db, "%d\n", country->city_costs_p[i][j]);
        }

        for(i = 0; i < country->ncities; i++) {
            for(j = 0; j < country->ncities - 1; j++) {
                fprintf(city_db, "%d ", country->city_distances_p[i][j]);
            }
            fprintf(city_db, "%d\n", country->city_distances_p[i][j]);
        }

        update_city_list(country->city_list, city_db);

        fclose(city_db);
    }
}
