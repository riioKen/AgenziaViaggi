#include "header.h"

int main() {
    user_t *user_list = NULL,
        *user = NULL;
    country_t *country = NULL;
    int flag = 0;

    user_list = fetch_users();
    country = init_country();

    do {
        clear_terminal();
        flag = main_menu();
        switch(flag) {
            case 1:
                user = sign_in(user_list);
                if(user != NULL) {
                    user_control_panel(user, country);
                    user = NULL;
                    update_user_list(user_list);
                }
                break;
            case 2:
                user_list = sign_up(user_list);
                break;
            case 3:
                if(login_admin()) {
                    admin_control_panel(country);
                }
                break;
            case 0:
                clear_terminal();
                printf("+-------------------------------+\n");
                printf("|    Arrivederci e a presto!    |\n");
                printf("+-------------------------------+\n");
                break;
            default:
                wrong_selection_message();
                break;
        }
    } while(flag);

    free_user_list(user_list);
    free_country(country);
    user_list = NULL;
    country = NULL;
    return 0;
}
