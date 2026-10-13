#include<stdio.h>
#include<string.h>

const int STANDARD=150;
const int PREMIUM=250;
const int RECLINER=400;

double calculate_discount(int ticket_count,double sub_total){
    if(ticket_count < 5) return 0.0;

    else{
        return 10.0 * (sub_total / 100.0);
    }
}

int get_category_price(char category[]){
    if(strcmp(category,"Standard") == 0) return STANDARD;

    else if(strcmp(category,"Premium") == 0) return PREMIUM;

    else return RECLINER;
}

int main(){
    char name[100];

    int ticket_count,category_num;
    char category[100];

    printf("Enter your name: ");
    scanf("%99s",name);

    printf("\nEnter number of tickets you want to book: ");
    scanf("%d",&ticket_count);

    if(ticket_count > 20 || ticket_count < 1){
        printf("Error: Ticket quantity should be from 1 to 20 only");
        return 1;
    }

    printf("\nSelect ticket category\n1. Standard - %dRS\n2. Premium - %dRS \n3. Recliner - %dRS\n=> ",STANDARD,PREMIUM,RECLINER);
    scanf("%d",&category_num);

    switch(category_num){
        case 1: strcpy(category,"Standard");
        break;

        case 2: strcpy(category,"Premium");
        break;

        case 3: strcpy(category,"Recliner");
        break;

        default: printf("Error: please select correct category");
        return 1;
    }

    double category_price=get_category_price(category);

    double sub_total=category_price * ticket_count;

    double discount=calculate_discount(ticket_count,sub_total);

    double final_bill=sub_total - discount;

    printf("\nCustomer: %s\n",name);
    printf("Ticket Category: %s\n",category);
    printf("Tickets: %d\n",ticket_count);
    printf("Subtotal: %.2lf\n",sub_total);
    printf("Discount: %.2lf\n",discount);
    printf("Final Bill: %.2lf",final_bill);

    return 0;

}