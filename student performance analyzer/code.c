
#include<stdio.h>

typedef struct Student{
    char name[200];
    int roll_no;
    int sub1_marks;
    int sub2_marks;
    int sub3_marks;
} Student;

int get_total_marks(int marks1,int marks2,int marks3){
    return marks1 + marks2 + marks3;
}

float get_avg_marks(int marks1,int marks2,int marks3){
    return (marks1 + marks2 + marks3) / 3.0f;
}

char assign_grade(float avg_marks){
    if(avg_marks >= 85) return 'A';
    else if(avg_marks >= 70) return 'B';
    else if(avg_marks >= 50) return 'C';
    else if(avg_marks >= 35) return 'D';
    else return 'F';
}

void print_performance(char grade){

    switch(grade){
        case 'A': printf("Performance: *****\n");
        break;

        case 'B': printf("Performance: ****\n");
        break;

        case 'C': printf("Performance: ***\n");
        break;

        case 'D': printf("Performance: **\n");
        break;

        default: break;
    }

    printf("\n");
}

void print_roll_numbers(Student students[],int index){
    if(index < 0){
        printf("List of Roll Numbers (via recursion): ");
        return;
    }

    print_roll_numbers(students,index-1);

    printf("%d ",students[index].roll_no);
}

int main(){

    Student students[100];
    int n;

    printf("Enter number of students: ");

    if(scanf("%d",&n) != 1 || n < 1 || n > 100){
        printf("Please enter a valid number of students 1-100.\n");
        return 1;
    }

    getchar();

    printf("\n");

    for(int i=0; i < n; i++){

        printf("\nEnter these details using spaces as separators for student %d:\n",i+1);
        printf("Roll Number | Name | Marks 1 | Marks 2 | Marks 3\n");

        int count = scanf("%d %199s %d %d %d",
            &students[i].roll_no,
            students[i].name,
            &students[i].sub1_marks,
            &students[i].sub2_marks,
            &students[i].sub3_marks);

        if(count != 5){
            printf("\nInvalid or incomplete student details. Please enter all 5 details.\n");
            return 1;
        }

        if(students[i].roll_no <= 0){
            printf("Please enter a valid roll number.\n");
            return 1;
        }

        if(students[i].sub1_marks > 100 || students[i].sub1_marks < 0 ||
           students[i].sub2_marks > 100 || students[i].sub2_marks < 0 ||
           students[i].sub3_marks > 100 || students[i].sub3_marks < 0){

            printf("\nMarks can only be in range 0-100, please try again.\n");
            return 1;
        }
    }

    printf("\n");

    for(int i=0; i < n; i++){

        int total_marks = get_total_marks(
            students[i].sub1_marks,
            students[i].sub2_marks,
            students[i].sub3_marks);

        float avg_marks = get_avg_marks(
            students[i].sub1_marks,
            students[i].sub2_marks,
            students[i].sub3_marks);

        char grade = assign_grade(avg_marks);

        printf("Roll: %d\n",students[i].roll_no);
        printf("Name: %s\n",students[i].name);
        printf("Total: %d\n",total_marks);
        printf("Average: %.2f\n",avg_marks);
        printf("Grade: %c\n",grade);

        if(grade == 'F'){
            printf("\n");
            continue;
        }

        print_performance(grade);
    }

    print_roll_numbers(students,n-1);

    return 0;
}
