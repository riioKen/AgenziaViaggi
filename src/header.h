#ifndef HEADER_H
#define HEADER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Librerie necessarie per sleep() e Sleep()
 */
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define ADMIN_PIN 1234
#define DEFAULT_SLEEP 2
#define LINE_MAX 150
#define MAX 20
#define MAX_LONG 50

#define CITY_DB "database/city_list.txt"
#define REPORTS_DB "database/reports.txt"
#define USER_DB "database/users.txt"

typedef struct city {
    int npoi;                       // # Punti di Interesse
    int **poi;                      // Distanze dei Punti di Interesse
    char **poi_names;               // Nomi dei Punti di Interesse
    struct city *next;
} city_t;

typedef struct country {
    int ncities;                    // # Città in un Paese
    int **city_costs_t;             // Costi di viaggio tra le Città in treno
    int **city_distances_t;         // Distanze tra le Città in treno
    int **city_costs_p;             // Costi di viaggio tra le città in aereo
    int **city_distances_p;         // Distanze tra le città in aereo
    char **cities_names;            // Nome delle città
    city_t *city_list;
} country_t;

// Lista Single Linked per gli Utenti
typedef struct user {
    char first_name[MAX_LONG];      // Nome
    char last_name[MAX_LONG];       // Cognome
    char email[MAX_LONG];           // Email
    char password[MAX_LONG];        // Password
    float balance;                  // Saldo Account
    struct user *next;
} user_t;

///////////////////////////////////////
//      Funzioni Amministratore      //
///////////////////////////////////////

void add_link(country_t *country, int start, int end, int is_plane);
void add_manual_link(country_t *country);
void admin_control_panel(country_t *country);
void check_reports(country_t *country, int **reports, int reports_amount);
int **fetch_reports(int dim);
int fetch_reports_amount();
int login_admin();
void remove_city(country_t *country);

//////////////////////////////
//      Funzioni Città      //
//////////////////////////////

city_t *add_city_to_list(city_t *city_list, city_t *new);
city_t *fetch_cities(FILE *city_db, int ncities);
void free_city_list(city_t *city_list);
void free_city_matrix(city_t *city);
int get_city_index(country_t *country, char city_name[]);
city_t *new_city();
city_t *remove_city_from_list(city_t *city_list, int index);
void update_city_list(city_t *city_list, FILE *city_db);

//////////////////////////////
//      Funzioni Paese      //
//////////////////////////////

int check_city(country_t *country, char city_name[]);
char **create_char_matrix(int row, int length);
int **create_int_matrix(int dim);
void free_country(country_t *country);
void free_country_matrix(country_t *country);
country_t *init_country();
country_t *new_country();
void update_country_db(country_t *country);

/////////////////////////////////////
//      Funzioni Prenotazione      //
/////////////////////////////////////

void booking_main(user_t *user, country_t *country);
void book_hotel(country_t *country, int city_index, int is_plane);
int check_link(int **distance_matrix, int dim, int city_index);
void dijkstra_city(city_t *city, int start, int end);
float dijkstra_cost(country_t *country, int **cost_matrix, int **distance_matrix, int start, int end);
float dijkstra_distance(country_t *country, int **cost_matrix, int **distance_matrix, int start, int end);
int get_minimum_distance(int dim, int distance[], bool done[]);
void print_shortest_path(char **names, int distance[], int previous[], int start, int end);
void print_path(char **names, int previous[], int index);
void report_missing_link(int start, int end, int is_plane);
int transport_choice();
int travel_by_plane(user_t *user, country_t *country);
int travel_by_train(user_t *user, country_t *country);

///////////////////////////
//      Funzioni UI      //
///////////////////////////

void clear_terminal();
void csleep(int seconds);
int main_menu();
void print_divider();
void print_logo();
void wrong_selection_message();

///////////////////////////////
//      Funzioni Utente      //
///////////////////////////////

user_t *add_user_to_list(user_t *users_list, user_t *new);
void balance_management(user_t *user);
void balance_top_up(user_t *user);
int check_email(user_t *user_list, char email[]);
int check_password(user_t *user_list, char email[], char password[]);
user_t *fetch_users();
void free_user_list(user_t *user_list);
user_t *get_user(user_t *user_list, char email[]);
user_t *sign_in(user_t *user_list);
user_t *sign_up(user_t *user_list);
user_t *new_user(char first_name[], char last_name[], char email[], char password[], float balance);
void update_user_list(user_t *user_list);
void user_control_panel(user_t *user, country_t *country);

#endif
