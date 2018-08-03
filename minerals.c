#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



// ===========================================================================
// =========================== DEFINICJA LIST ================================
// ===========================================================================


struct Mineral
{
    char *name;
    char *number;
    char *composition;
    struct Mineral *next;
};
struct sBranch
{
    char *branch_name;
    struct sBranch *next;
    struct Mineral *mineral;
};


// ===========================================================================
// =========================== FUNKCJE POMOCNICZE ============================
// ===========================================================================


//Funkcja sprawdzajaca istnienie katalogu o podanej nazwie
int if_branch_exist(struct sBranch *head, char *text)
{
    struct sBranch *branch = head;
    while(branch != NULL)
    {
        if (strcmp(branch->branch_name,text) == 0)
            return 1;

        branch = branch->next;
    }

    return 0;
}

//Funkcja sprawdzajaca istnienie mineralu o podanym numerze
int if_record_exist(struct sBranch *head, char *number)
{
    struct sBranch *branch = head;
    struct Mineral *mineral;
    while(branch != NULL)
    {
        mineral = branch->mineral;
        while(mineral != NULL)
        {            
            if (strcmp(mineral->number,number) == 0)
                return 1;
            mineral = mineral->next;
        }
        branch = branch->next;
    }

    return 0;
}

//Usuwamy znak konca linii ze stringa
void change_string(ssize_t *read_length, char** name)
{
    int i=0;
    *read_length -= 1;
    char* help = (char*)malloc(sizeof(*read_length));
    for(i=0; i<(*read_length); i++)
        help[i] = (*name)[i];
    free(*name);
    *name = help;
}

//Funkcja czyszczaca bufor
void myFlush(void)
{
    int c;
    while ((c = fgetc(stdin)) != EOF && c!='\n');
}

//Funkcja zmieniajaca wprowadzony w stringu znaku enter na znak '\0', oznaczajacy koniec stringa
void changeString (char variable[], int max_size)
{
    int size = 0;
    for (size=0; size<max_size; size++)
        if (variable[size] == '\n')
        {
            variable[size] = '\0';
            return ;
        }
    myFlush();
}

//Funkcja zliczajaca ilosc elementow w stringu
int getStringLength (char variable[], int max_size)
{
    int size = 0;
    for (size=0; size<max_size; size++)
        if (variable[size] == '\0') break;
    return size;
}

// ===========================================================================
// ========================== FUNKCJE GŁÓWNE =================================
// ===========================================================================


//Funkcja dodajaca nowy katalog poprzez nazwe
void add_branch(struct sBranch **head, char *new_branch_name)
{
    if (*head == NULL)
    {
        (*head) = (struct sBranch*) malloc (sizeof(struct sBranch));
        (*head)->next = NULL;
        (*head)->mineral = NULL;
        (*head)->branch_name = new_branch_name;
        return;
    }

    if (strcmp((*head)->branch_name, new_branch_name)>0)
    {
        struct sBranch *new_branch = (struct sBranch*) malloc (sizeof(struct sBranch));
        new_branch->next = (*head);
        new_branch->branch_name = new_branch_name;
        new_branch->mineral = NULL;
        *head = new_branch;
        return;
    }

    struct sBranch *help = (*head);

    while(help->next != NULL && strcmp(help->next->branch_name, new_branch_name)<0)
        help = help->next;

    if (help->next != NULL && strcmp(help->next->branch_name, new_branch_name) == 0)
    {
        printf("Nazwa katalogu istnieje już w bazie!\n");
        return;
    }

    struct sBranch *new_branch = (struct sBranch*) malloc (sizeof(struct sBranch));
    new_branch->next = help->next;
    new_branch->branch_name = new_branch_name;
    new_branch->mineral = NULL;
    help->next = new_branch;
}

//Zmiana nazwy katalogu
void change_branch_name(struct sBranch **head, char* old_name, char* new_name)
{
    struct sBranch *target = (*head);
    while(target != NULL)
    {
        if (strcmp(target->branch_name,old_name)==0)
        {
            target->branch_name = new_name;
            free(old_name);
            break;
        }
        else target = target->next;
    }
}

//Usuniecie katalogu o podanej nazwie
void delete_branch(struct sBranch **head, char* name)
{
    struct sBranch *tmp;
    struct sBranch *help = (*head);
    struct Mineral *temp;

    //Katalog do usuniecia to glowa
    if(strcmp(help->branch_name,name)==0)
    {
        while((*head)->mineral != NULL)
        {
            temp = (*head)->mineral;
            (*head)->mineral = (*head)->mineral->next;
            free(temp->name);
            free(temp->number);
            free(temp->composition);
            free(temp);
        }
        (*head) = help->next;
        free(help);


    //Katalog do usuniecia to nie glowa
    } else
    {
        while(help->next != NULL)
        {
            //Jezeli znaleziono katalog to usun branch
            if(strcmp(help->next->branch_name,name)==0)
            {
                while(help->next->mineral != NULL)
                {
                    temp = help->next->mineral;
                    help->next->mineral = help->next->mineral->next;
                    free(temp->name);
                    free(temp->number);
                    free(temp->composition);
                    free(temp);
                }
                tmp = help->next;
                help->next = help->next->next;
                free(tmp);
                break;
            }
            help = help->next;
        }
    }
}

//Dodanie rekordu
void add_record(struct sBranch *head, char *branch_name, char *mineral_name, char *mineral_number, char *mineral_composition)
{
    struct sBranch *help_branch = head;
    struct Mineral *help_mineral;

    //Nadajemy wartosci nowemu rekordowi
    struct Mineral *new_mineral = (struct Mineral*) malloc (sizeof(struct Mineral));
    new_mineral->name = mineral_name;
    new_mineral->number = mineral_number;
    new_mineral->composition = mineral_composition;
    new_mineral->next = NULL;

    //Znajdz dzial
    while (strcmp(help_branch->branch_name,branch_name)!=0)
        help_branch = help_branch->next;
    help_mineral = help_branch->mineral;

    //Jezeli nie ma mineralow
    if(help_mineral == NULL)
        help_branch->mineral = new_mineral;

    //Jezeli nazwa mineralu jest mniejsza niz nazwa glowy
    else if (strcmp(help_mineral->name, mineral_name)>0)
    {
        help_branch->mineral = new_mineral;
        new_mineral->next = help_mineral;
    }

    //Jezeli nazwa mineralu jest wieksza niz nazwa glowy
    else
    {
        //Znajdz ostatni element mniejszy niz mineral
        while(help_mineral->next != NULL && strcmp(help_mineral->next->name, mineral_name)<0)
            help_mineral = help_mineral->next;

        //Podepnij
        new_mineral->next = help_mineral->next;
        help_mineral->next = new_mineral;
    }
}

//Funkcja usuwajaca cala liste
void delete_list(struct sBranch **head)
{
    struct sBranch *branch = (*head);
    while (branch != NULL)
    {
        branch = branch->next;
        free(*head);
        (*head) = branch;
    }
}

// ===========================================================================
// ======================== FUNKCJE UŻYTKOWNIKA ==============================
// ===========================================================================


//Dodanie katalogu
void user_add_branch(struct sBranch **head)
{
    int i;
    char* name;
    size_t read_all_name = 0;
    ssize_t read_length_name;

    //Pobierz nazwe katalogu do dodania
    printf("Podaj nazwę katalogu, jaki chcesz dodac: ");
    read_length_name = getline(&name, &read_all_name, stdin);
    change_string(&read_length_name, &name);

    //Sprawdz poprawnosc nazwy katalogu
    for(i=0; i<read_length_name; i++)
    if (isalpha(name[i])==0 && isspace(name[i])==0)
    {
        printf("Bledne dane - niepoprawny znak w nazwie katalogu!\n ");
        free(name);
        return;
    }

    //Jezeli istnieje
    if (!if_branch_exist((*head),name))
        add_branch(head,name);
    else printf("Podana nazwa katalogu istnieje juz w bazie!\n");
}

//Zmiana nazwy katalogu
void user_change_branch_name(struct sBranch **head)
{
    char* old_name;
    size_t read_all_old = 0;
    ssize_t read_length_old;

    char* new_name;
    size_t read_all_new = 0;
    ssize_t read_length_new;

    //Pobierz nazwe katalogu do zmiany
    printf("Podaj nazwę katalogu, którą chcesz zmienić: ");
    read_length_old = getline(&old_name, &read_all_old, stdin);
    change_string(&read_length_old, &old_name);

    //Jezeli nie ma takiego katalogu
    if (!if_branch_exist((*head),old_name))
    {
        printf("Podana nazwa katalogu nie istnieje!\n");
        free(old_name);
        return;
    }

    //Pobierz nowa nazwe katalogu
    printf("Podaj nową nazwę katalogu: ");
    read_length_new = getline(&new_name, &read_all_new, stdin);
    change_string(&read_length_new, &new_name);

    //Jezeli jest taki katalog
    if (if_branch_exist((*head),new_name))
    {
        printf("Podana nazwa katalogu istnieje juz w bazie!");
        free(old_name);
        free(new_name);
        return;
    }

    //Zmien nazwe
    change_branch_name(head, old_name, new_name);
}

//Usuniecie katalogu
void user_delete_branch(struct sBranch **head)
{
    char* name;
    size_t read_all_name = 0;
    ssize_t read_length_name;

    //Pobierz nazwe katalogu do dodania
    printf("Podaj nazwę katalogu, ktory chcesz usunac: ");
    read_length_name = getline(&name, &read_all_name, stdin);
    change_string(&read_length_name, &name);

    //Jezeli istnieje
    if (if_branch_exist((*head),name))
        delete_branch(head,name);
    else printf("Katalog o podanej nazwie nie istnieje!");

    //Zwalnianie zasobow
    free(name);
}

//Dodanie rekordu
void user_add_record(struct sBranch *head)
{
    int i=0;
    char* branch_name, *mineral_name, *number, *composition;
    size_t read_all_branch = 0, read_all_mineral = 0, read_all_number = 0, read_all_composition = 0;
    ssize_t read_length_branch, read_length_mineral, read_length_number, read_length_composition;

    //Pobierz nazwe katalogu
    printf("Podaj nazwe katalogu, do ktorego chcesz dodac rekord: ");
    read_length_branch = getline(&branch_name, &read_all_branch, stdin);
    change_string(&read_length_branch, &branch_name);

    //Jezeli nie ma takiego katalogu
    if (!if_branch_exist(head,branch_name))
    {
        printf("Podana nazwa katalogu nie istnieje!\n");
        free(branch_name);
        return;
    }

    //Pobierz nazwe mineralu
    printf("Podaj nazwe mineralu (malymi literami, nazwa moze zawierac spacje): ");
    read_length_mineral = getline(&mineral_name, &read_all_mineral, stdin);
    change_string(&read_length_mineral, &mineral_name);

    //Sprawdz poprawnosc nazwy mineralu
    for(i=0; i<read_length_mineral; i++)
    if (islower(mineral_name[i])==0 && isspace(mineral_name[i])==0)
    {
        printf("Bledne dane - niepoprawny znak w nazwie mineralu! ");
        free(branch_name);
        free(mineral_name);
        return;
    }

    //Pobierz numer mineralu
    printf("Podaj pieciocyfrowy numer mineralu: ");
    read_length_number = getline(&number, &read_all_number, stdin);
    change_string(&read_length_number, &number);

    //Sprawdz, czy jest odpowiedniej dlugosci
    if (read_length_number!=5)
    {
        printf("Podany numer nie jest pieciocyfrowy!\n");
        free(branch_name);
        free(mineral_name);
        free(number);
        return;
    }

    //Sprawdz, czy numer jest poprawny
    for(i=0; i<read_length_number; i++)
    if (isdigit(number[i])==0)
    {
        printf("Bledne dane - niepoprawny znak w numerze mineralu!\n");
        free(branch_name);
        free(mineral_name);
        free(number);
        return;
    }

    //Sprawdz, czy numer jest unikalny
    if (if_record_exist(head,number))
    {
        printf("Podany numer jest juz w bazie!\n");
        free(branch_name);
        free(mineral_name);
        free(number);
        return;
    }

    //Sprawdz, czy numer jest unikalny
    printf("Podaj sklad chemiczny mineralu: ");
    read_length_composition = getline(&composition, &read_all_composition, stdin);
    change_string(&read_length_composition, &composition);

    //Sprawdz, czy sklad jest poprawny
    for(i=0; i<read_length_composition; i++)
    if (isalnum(composition[i])==0)                                                      //Jesli znak inny niz cyfra lub litera
    {
        printf("Bledne dane - niepoprawny znak w skladzie mineralu!\n");
        free(branch_name);
        free(mineral_name);
        free(number);
        free(composition);
        return;
    }

    //Dodanie mineralu
    add_record(head,branch_name,mineral_name,number,composition);
}

//Przenoszenie rekordu
void user_move_record(struct sBranch *head)
{
    struct sBranch *source_branch = head;
    struct sBranch *destination_branch = head;
    struct Mineral *source_mineral, *source_mineral_help;
    struct Mineral *destination_mineral;

    char *source_name, *destination_name, *option, *number;
    size_t read_all_source = 0, read_all_destination = 0, read_all_option = 0, read_all_number = 0;
    ssize_t read_length_source, read_length_destination, read_length_option, read_length_number;

    //Pobieramy nazwe katalogu zrodlowego
    printf("Podaj nazwę katalogu, z ktorego chcesz przeniesc rekordy: ");
    read_length_source = getline(&source_name, &read_all_source, stdin);
    change_string(&read_length_source, &source_name);

    //Jezeli nie istnieje
    if (!if_branch_exist(head, source_name))
    {
        printf("Podana nazwa katalogu nie istnieje!\n");
        free(source_name);
        return ;
    }

    //Przechodzimy po liscie do katalogu zrodlowego
    while (strcmp(source_branch->branch_name,source_name)!=0)
        source_branch = source_branch->next;

    //Jezeli katalog nie posiada zadnych rekordow
    if (source_branch->mineral==NULL)
    {
        printf("Nie ma zadnych rekordow do przeniesienia!\n");
        free(source_name);
        return;
    }

    //Pobieramy nazwe katalogu docelowego
    printf("Podaj nazwe katalogu, do ktorego chcesz przeniesc rekordy: ");
    read_length_destination = getline(&destination_name, &read_all_destination, stdin);
    change_string(&read_length_destination, &destination_name);

    //Jezeli nie istnieje
    if (!if_branch_exist(head, destination_name))
    {
        printf("Podana nazwa katalogu nie istnieje!\n");
        free(source_name);
        free(destination_name);
        return ;
    }

    //PRzechodzimy po liscie do katalogu docelowego
    while (strcmp(destination_branch->branch_name,destination_name)!=0)
        destination_branch = destination_branch->next;

    //Pytamy o opcje
    printf("Chcesz przeniesc wybrany rekord (0), czy wszystkie rekordy(1)?: ");
    read_length_option = getline(&option, &read_all_option, stdin);
    change_string(&read_length_option, &option);

    //Jezli przenosimy jeden rekord
    if (strcmp(option,"0")==0)
    {
        //Pobieramy numer mineralu
        printf("Podaj numer mineralu, ktory chcesz przeniesc: ");
        read_length_number = getline(&number, &read_all_number, stdin);
        change_string(&read_length_number, &number);

        //Mineral nie istnieje
        if (!if_record_exist(head,number))
        {
            printf("Rekord o podanym numerze nie istnieje!\n");
            free(source_name);
            free(destination_name);
            free(number);
            return ;
        }

        //Znajdujemy mineral do przeniesienia
        source_mineral = source_branch->mineral;

        //Jezeli przenosimy mineral z glowy
        if (strcmp(source_mineral->number,number)==0)
        {
            source_branch->mineral = source_mineral->next;
            source_mineral->next = NULL;

        //Jezeli to nie jest jedyny mineral brancha source
        } else if (source_mineral->next != NULL)
        {
            //Znajdz mineral
            while (source_mineral->next!=NULL && strcmp(source_mineral->next->number,number)!=0)
                source_mineral=source_mineral->next;

            //Nie ma takiego mineralu!
            if (source_mineral->next == NULL)
            {
                printf("Nie ma mineralu o podanym numerze w danym katalogu!\n");
                free(source_name);
                free(destination_name);
                free(option);
                free(number);
                return;
            }

            //Odlacz mineral od listy
            source_mineral_help = source_mineral;
            source_mineral = source_mineral->next;
            source_mineral_help->next = source_mineral->next;
            source_mineral->next = NULL;

        //Jezeli to jedyny mineral
        } else
        {
            printf("Nie ma mineralu o podanym numerze!\n");
            free(source_name);
            free(destination_name);
            free(option);
            free(number);
            return;
        }

        //Jezeli w destination nic nie ma
        if (destination_branch->mineral == NULL)
            destination_branch->mineral = source_mineral;
        else
        {
            //Przejdz do konca listy
            destination_mineral = destination_branch->mineral;
            while(destination_mineral->next != NULL)
                destination_mineral=destination_mineral->next;

            //Podepnij
            source_mineral->next = destination_mineral->next;
            destination_mineral->next = source_mineral;


        }

        free(source_name);
        free(destination_name);
        free(option);
        free(number);

    //Jezeli przenosimy wszystkie rekordy
    } else if (strcmp(option,"1")==0)
    {
        //Jezeli w destination nic nie ma
        if (destination_branch->mineral == NULL)
        {
            destination_branch->mineral = source_branch->mineral;
            source_branch->mineral = NULL;
        } else
        {
            //Przejdz do konca listy
            destination_mineral = destination_branch->mineral;
            while(destination_mineral->next != NULL)
                destination_mineral=destination_mineral->next;

            //Podepnij
            destination_mineral->next = source_branch->mineral;
            source_branch->mineral = NULL;
        }

        free(source_name);
        free(destination_name);
        free(option);
        return;

    //Jezeli zla opcja
    } else
    {
        printf("Wybrano nieprawidlowa opcje!\n");
        free(source_name);
        free(destination_name);
        free(option);
    }
}

//Zmiana rekordu
void user_change_record(struct sBranch *head)
{
    struct sBranch *branch = head;
    struct Mineral *mineral;

    char* branch_name, *number, *option;
    size_t read_all_branch = 0, read_all_number = 0, read_all_option = 0;
    ssize_t read_length_branch, read_length_number, read_length_option;

    //Pobieramy nazwe katalogu
    printf("Podaj nazwę katalogu, w ktorym chcesz zmienic rekord: ");
    read_length_branch = getline(&branch_name, &read_all_branch, stdin);
    change_string(&read_length_branch, &branch_name);

    //Jezeli nie istnieje to koniec
    if (!if_branch_exist(head,branch_name))
    {
        printf("Podana nazwa katalogu nie istnieje!\n");
        free(branch_name);
        return;
    }

    //Przechodzimy po liscie do katalogu
    while (strcmp(branch->branch_name,branch_name)!=0)
        branch = branch->next;

    //Jezeli katalog nie posiada zadnych rekordow
    if (branch->mineral==NULL)
    {
        printf("Nie ma zadnych rekordow do zmiany!\n");
        free(branch_name);
        return;
    }

    //Pobieramy numer mineralu
    printf("Podaj numer mineralu, ktory chcesz zmienic: ");
    read_length_number = getline(&number, &read_all_number, stdin);
    change_string(&read_length_number, &number);

    //Nie ma takiego mineralu
    if (!if_record_exist(head, number))
    {
        printf("Nie ma mineralu o podanym numerze!\n");
        free(branch_name);
        free(number);
        return;
    }

    //Znajdujemy mineral do zmiany
    mineral = branch->mineral;
    while (mineral!=NULL && strcmp(mineral->number,number)!=0)
        mineral=mineral->next;

    //Jest mineral, ale w innym branchu
    if (mineral==NULL)
    {
        printf("Nie ma mineralu o podanym numerze w danym katalogu!\n");
        free(branch_name);
        free(number);
        return;
    }

    //Pytamy co chce zmienic
    printf("Chcesz zmienic nazwe mineralu(1), numer mineralu(2), sklad(3) czy wszystkie pozycje(0)?: ");
    read_length_option = getline(&option, &read_all_option, stdin);
    change_string(&read_length_option, &option);

    //Jezeli zmieniamy nazwe
    if (strcmp(option,"1")==0)
    {
        //Zmienamy nazwe mineralu
        printf("Podaj nowa nazwe mineralu: ");
        ssize_t read_length_mineral = getline(&mineral->name, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->name));

    //Jezeli zmieniamy numer
    } else if (strcmp(option,"2")==0)
    {
        //Zmienamy numer mineralu
        printf("Podaj nowy numer mineralu: ");
        ssize_t read_length_mineral = getline(&mineral->number, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->number));

    //Jezeli zmieniamy sklad
    } else if (strcmp(option,"3")==0)
    {
        //Zmienamy sklad mineralu
        printf("Podaj nowy sklad mineralu: ");
        ssize_t read_length_mineral = getline(&mineral->composition, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->composition));

    //Jezeli zmieniamy wszystko
    } else if (strcmp(option,"0")==0)
    {
        //Zmienamy nazwe mineralu
        printf("Podaj nowa nazwe mineralu: ");
        ssize_t read_length_mineral = getline(&mineral->name, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->name));

        //Zmienamy numer mineralu
        printf("Podaj nowy numer mineralu: ");
        read_length_mineral = getline(&mineral->number, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->number));

        //Zmienamy sklad mineralu
        printf("Podaj nowy sklad mineralu: ");
        read_length_mineral = getline(&mineral->composition, &read_all_number, stdin);
        change_string(&read_length_mineral, &(mineral->composition));

    //Jezeli zla opcja
    } else printf("Wybrano nieprawidlowa opcje!\n");

    //Sprzatamy
    free(branch_name);
    free(number);
    free(option);
}


// ===========================================================================
// ============================== FUNKCJE FILE ===============================
// ===========================================================================

void load_data_from_file(struct sBranch **head)

{
    char* text;
    size_t read_all_name = 0;
    ssize_t length;
    FILE *fo=fopen("a.txt", "r");
    char* branch;

    //Dopoki plik sie nie skonczyl
    do
    {

        //Pobierz linie
        length = getline(&text, &read_all_name, fo);

        //Dopoki !EOF
        if (length != -1)
        {
            //Oblicz ilosc wyrazow
            int sum = 0, i;
            for (i=0; i<length; i++)
                if (text[i] == '[' || text[i] == ']') sum++;

            //Jezli tylko jeden wyraz (nowy dzial)
            if (sum==2)
            {
                int i=0;
                int start, stop;

                //Oblicz pozycje wyrazu
                for (i=0; i<length; i++)
                    if (text[i] == '[') start = i;
                    else if (text[i] == ']') stop = i+1;

                //Przydziel pamiec
                branch = (char*)malloc((stop-start-1)*sizeof(char));

                //Przepisz wyraz
                for (i=start; i<stop-1; i++)
                    branch[i-start-1] = text[i];

                //Dodaj branch
                add_branch(head,branch);

            } else if (sum == 6)
            {
                int i=0, j=0;
                    int start, stop;

                // Oblicz pozycje name
                for (; i<length; i++)
                    if (text[i] == '[') start = i+1;
                    else if (text[i] == ']')
                    {
                        stop = i-1;
                        break;
                    }
                //Przydziel pamiec
                char* name = (char*)malloc((stop-start-1)*sizeof(char));

                //Przepisz wyraz
                for (j=start; j<stop+1; j++)
                    name[j-start] = text[j];

                // Oblicz pozycje number
                for (i++; i<length; i++)
                    if (text[i] == '[') start = i+1;
                    else if (text[i] == ']')
                    {
                        stop = i-1;
                        break;
                    }
                //Przydziel pamiec
                char* number = (char*)malloc((stop-start-1)*sizeof(char));

                //Przepisz wyraz
                for (j=start; j<stop+1; j++)
                    number[j-start] = text[j];

                // Oblicz pozycje composition
                for (i++; i<length; i++)
                    if (text[i] == '[') start = i+1;
                    else if (text[i] == ']')
                    {
                        stop = i-1;
                        break;
                    }

                //Przydziel pamiec
                char* composition = (char*)malloc((stop-start-1)*sizeof(char));

                //Przepisz wyraz
                for (j=start; j<stop+1; j++)
                    composition[j-start] = text[j];

                //Dodaj rekordy
                add_record(*head,branch,name,number,composition);
                //printf("%s\t",name);
                //printf("%s\t",number);
                //printf("%s\n",composition);


            } else
            {
                //blad
                printf ("Niepoprawna zawartosc pliku!\n");
            }
        }
    } while(length != -1);

    fclose (fo);

}


// ===========================================================================
// ============================ FUNKCJE DRUKUJACE ============================
// ===========================================================================


void print_branch_list(struct sBranch *head)                                            //Funkcja drukujaca liste katalogow
{
    struct sBranch *branch = head;
    while (branch != NULL)
    {
        printf("%s\n",branch->branch_name);
        branch = branch->next;
    }
    return;
}
void print_branch_records(struct sBranch *head, char *branch_name)                      //Funkcja drukujaca liste rekordow danego brancha
{
    struct sBranch *help = head;
    while(strcmp(help->branch_name,branch_name) != 0)
        help = help->next;

    if (help->mineral == NULL)
    {
         printf("Katalog o nazwie %s jest pusty!\n",help->branch_name);
         return;
    }
    else
    {
        printf("%s\t",help->branch_name);
        printf("%s\t",help->mineral->name);
        printf("%s\t",help->mineral->number);
        printf("%s\n",help->mineral->composition);
    }
    struct Mineral *tmp = help->mineral;
    while (tmp->next != NULL)
    {
        printf("\t");
        printf("%s\t",tmp->next->name);
        printf("%s\t",tmp->next->number);
        printf("%s\n",tmp->next->composition);
        tmp = tmp->next;
    }
}
void print_records(struct sBranch *head)                                                //Funkcja drukujaca liste rekordow
{
    char* branch_name;
    size_t read_all_branch = 0;
    ssize_t read_length_branch;

    printf("Podaj nazwe katalogu, ktorego zawartosc chcesz wyswietlic: ");
    //print_branch_list(head);
    read_length_branch = getline(&branch_name, &read_all_branch, stdin);
    change_string(&read_length_branch, &branch_name);

    if (if_branch_exist(head,branch_name) == 1)
        print_branch_records(head,branch_name);
    else printf("Podany katalog nie istnieje!\n");
}


// ===========================================================================
// ================================== MAIN ===================================
// ===========================================================================

int main(void)
{
    //Stworz wskaznik do poczatku glownej listy
    struct sBranch *data_base = NULL;
    char option[3];

    printf("Program pozwala na stworzenie bazy z danymi o mineralach\n");

    while(1)
    {
        printf("%s%s%s%s%s%s%s%s%s%s",
            "1- Dodaj nowy katalog\n",
            "2- Zmien nazwe istniejacego katalogu\n",
            "3- Usun katalog\n",
            "4- Wysweietl liste katalogow\n",
            "5- Dodaj rekord do katalogu\n",
            "6- Przenies rekordy pomiedzy katalogami\n",
            "7- Zmien dane dotyczace rekordu\n",
            "8- Wyswietl dane zawarte w katalogu\n",
            "9- Wczytaj dane z pliku\n",
            "0- Wyjdz z programu\n");

        fgets(option, 3, stdin);
        changeString (option, 3);

        int size = getStringLength(option, 3);
        if ( size != 1 )
        {
            //Powrot do wprowadzania danych
            printf("Podales nieodpowiednia liczbe znakow!\n");
            continue;
        }
        if (option[0]=='1')
        {
            //Dodawanie katalogu
            user_add_branch(&data_base);
            continue;
        }
        if (option[0]=='2')
        {
            //Zmiana nazwy katalogu
            user_change_branch_name(&data_base);
            continue;
        }
        if (option[0]=='3')
        {
            //Usun katalog
            user_delete_branch(&data_base);
            continue;
        }
        if (option[0]=='4')
        {
            //Wyswietl liste katalogow
            print_branch_list(data_base);
            continue;
        }
        if (option[0]=='5')
        {
            //Dodaj rekord
            user_add_record(data_base);
            continue;
        }
        if (option[0]=='6')
        {
            //Przenies rekordy
            user_move_record(data_base);
            continue;
        }
        if (option[0]=='7')
        {
            //Zmien dane
            user_change_record(data_base);
            continue;
        }
        if (option[0]=='8')
        {
            //Wyswietl dane
            print_records(data_base);
            continue;
        }
        if (option[0]=='9')
        {
            //Wczytaj z pliku
            load_data_from_file(&data_base);
            continue;
        }
        if (option[0]=='0')
        {
            //Powtwierdzenie wyjscia z programu
            printf("Czy na pewno chcesz opuscic program?\n 1- Nie\n 2- Tak\n");
            fgets(option, 3, stdin);
            changeString (option, 3);

            int size = getStringLength(option, 3);
            if ( size != 1 )
            {
                printf("Podales nieodpowiednia liczbe znakow!\n");
                continue;
            }
            //Wyjscie z programu
            if ( option[0] == '2' ) exit(EXIT_SUCCESS);
            //Powrot do menu
            else if ( option[0] == '1' ) continue;
            else
            {
                printf("Wprowadziles bledny znak!\n");
                continue;
            }
        }
    }

    //Posprzataj
    delete_list(&data_base);

    return 0;
}

