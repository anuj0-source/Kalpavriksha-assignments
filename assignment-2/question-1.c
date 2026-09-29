#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct User
{
    int id;
    char name[100];
    int age;
};

void display()
{
    FILE *fptr = fopen("users.txt", "r");

    if (fptr == NULL)
    {
        printf("0 users found\n");
        return;
    }

    char line[100];
    while (fgets(line, 100, fptr) != NULL)
        printf("%s", line);

    fclose(fptr);
}

void write()
{
    FILE *fptr = fopen("users.txt", "a");
    FILE *ridptr = fopen("id_count.txt", "r");

    int id;

    if (ridptr == NULL)
    {

        id = 0;
    }
    else
    {
        char idArr[1000];
        fgets(idArr, 1000, ridptr);
        fclose(ridptr);
        int indx = 0;
        id = 0;
        while (idArr[indx] >= '0' && idArr[indx] <= '9')
        {
            id = id * 10 + (idArr[indx++] - '0');
        }
    }

    FILE *widptr = widptr = fopen("id_count.txt", "w");

    struct User u;

    u.id = id++;

    fprintf(widptr, "%d", id);
    fclose(widptr);

    char name[200];
    printf("Enter your name: ");
    fgets(name, 200, stdin);
    name[strcspn(name, "\n")] = '\0';

    strcpy(u.name, name);

    int age;
    printf("Enter your age: ");
    scanf("%d", &age);
    u.age = age;

    fprintf(fptr, "%d %s %d\n", u.id, u.name, u.age);

    printf("User added successfully with id: %d\n", id - 1);

    fclose(fptr);
}

void update()
{
    FILE *rptr = fopen("users.txt", "r");
    FILE *temp_ptr = fopen("temp.txt", "w");

    int id;
    printf("Enter user's id to update: ");
    scanf("%d", &id);

    if(rptr==NULL){
        printf("User not found\n");
        fclose(temp_ptr);
        remove("temp.txt");
        return;
    }

    char line[500];

    char curr_name[200];
    int curr_age = 0;

    bool found = false;

    while (fgets(line, 500, rptr) != NULL)
    {
        int curr_id = 0;

        int indx = 0;
        while (line[indx] != ' ')
            curr_id = curr_id * 10 + (line[indx++] - '0');

        if (curr_id != id)
        {
            fprintf(temp_ptr, "%s", line);
        }
        else
        {
            found = true;
            int i = 0;
            indx++;
            while (line[indx+1] < '0' || line[indx+1] > '9') curr_name[i++] = line[indx++];
            curr_name[i]='\0';
            indx++;

            while (line[indx] >= '0' && line[indx] <= '9')
                curr_age = curr_age * 10 + (line[indx++] - '0');
        }
    }

    if (found)
    {

        int action;
        printf("Choose an option:\n1. update name\n2. update age\n3. update both\n=>");
        scanf("%d", &action);

        struct User u;

        switch (action)
        {

        case 1:
        {
            char new_name[200];
            printf("Enter new name:");
            getchar();
            fgets(new_name, 200, stdin);
            new_name[strcspn(new_name, "\n")] = '\0';

            strcpy(u.name, new_name);

            u.id = id;
            u.age = curr_age;

            fprintf(temp_ptr,"%d %s %d\n", u.id, u.name, u.age);

            printf("Name updated successfully\n");
            break;
        }

        case 2:
        {
            int new_age;

            printf("Enter new age: ");
            scanf("%d", &new_age);

            u.id = id;
            strcpy(u.name, curr_name);
            u.age = new_age;

            fprintf(temp_ptr, "%d %s %d\n", u.id, u.name, u.age);
            printf("Age updated successfully\n");
            break;
        }

        case 3:
        {
            int new_age;
            char new_name[200];
            printf("Enter new name:");
            getchar();
            fgets(new_name, 200, stdin);

            new_name[strcspn(new_name, "\n")] = '\0';
            strcpy(u.name, new_name);

            printf("Enter new age:");
            scanf("%d", &new_age);

            u.id = id;
            u.age = new_age;
            fprintf(temp_ptr, "%d %s %d\n", u.id, u.name, u.age);
            printf("Name and age updated successfully\n");
            break;
        }

        default:{
            strcpy(u.name,curr_name);
            u.age=curr_age;
            u.id=id;

            fprintf(temp_ptr, "%d %s %d\n", u.id, u.name, u.age);
            printf("Failed to update user, press correct button\n");
            break;
        }
        }
    }

    else
    {
        printf("User not found\n");
    }

    fclose(rptr);
    fclose(temp_ptr);
    remove("users.txt");
    rename("temp.txt", "users.txt");
}

void delete(){
    FILE *rptr = fopen("users.txt", "r");

    if (rptr == NULL)
    {
        printf("0 users found, Unable to delete\n");
        return;
    }

    FILE *temp_file_ptr = fopen("temp.txt", "w");

    int id;

    printf("Enter user's id to delete: ");
    scanf("%d", &id);
    getchar();

    char line[500];

    bool found = false;
    int count = 0;

    while (fgets(line, 500, rptr) != NULL)
    {
        int curr_id = 0;

        int indx = 0;
        while (line[indx] != ' ')
            curr_id = curr_id * 10 + (line[indx++] - '0');

        if (curr_id != id)
        {
            fprintf(temp_file_ptr, "%s", line);
            count++;
        }
        else
            found = true;
    }

    if (count == 0)
    {
        FILE *id_count_ptr = fopen("id_count.txt", "w");
        fprintf(id_count_ptr, "%d", 0);
        fclose(id_count_ptr);
    }

    fclose(rptr);
    fclose(temp_file_ptr);

    remove("users.txt");
    if(count == 0) remove("temp.txt");
    else rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted successfully\n");
    else
        printf("Unable to find user\n");
}

void printOptions(){
    printf("Choose an operation:\n1. Create an user\n2. Display users\n3. Update an user\n4. Delete an user\n5. Exit\n=> ");
}

int main(){
    while(true){

        int option;
        printOptions();
        scanf("%d",&option);
        getchar();

        switch(option){
            case 1:{
                write();
                break;
            }
            
            case 2:{
                display();
                break;
            }

            case 3:{
                update();
                break;
            }

            case 4:{
                delete();
                break;
            }
            
            case 5:{
                printf("Exitted");
                return 0;
            }

            default:{
                printf("Please select correct option\n");
                break;
            }
        }
    }
}