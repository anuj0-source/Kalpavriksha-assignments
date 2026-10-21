#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<limits.h>

int noOfEmployees=0;

typedef struct Employee Employee;

struct Employee{

    int id;
    char name[100];
    char designation[100];
    Employee **reports;
    int reportsCount;
    Employee *reporting_manager;

};

void printMenu(){
    printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT SYSTEM =====\n\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Heirarchy\n");
    printf("5. Exit\n\n");
    printf("=> ");
}

Employee *getEmployee(int id,Employee **employees){

    for(int i=0; i < noOfEmployees; i++){
        if(employees[i]->id == id) return employees[i];
    }

    return NULL;
}

bool exists(int id,Employee **employees){

    for(int i=0; i < noOfEmployees; i++){
        Employee curr=*employees[i];

        if(curr.id == id) return true;
    }

    return false;
}

bool isReportingCycle(int currEmpId,Employee *reportingManager){
    if(reportingManager == NULL) return false;
    if(reportingManager->id == currEmpId) return true;

    return isReportingCycle(currEmpId,reportingManager->reporting_manager);
}

int addEmployee(Employee ***employees){
    int id;

    retryId:
    printf("Enter id of employee: ");
    scanf("%d",&id);

    if(exists(id,*employees)){
        printf("Id already exists please enter unique id\n");
        goto retryId;
    }

    *employees=realloc(*employees,(noOfEmployees + 1) * sizeof(Employee *));

    (*employees)[noOfEmployees]=calloc(1,sizeof(Employee));

    (*employees)[noOfEmployees]->id=id;

    printf("Enter name of the employee: ");
    scanf("%s",(*employees)[noOfEmployees]->name);
    getchar();

    printf("Enter designation of the employee: ");
    fgets((*employees)[noOfEmployees]->designation,sizeof((*employees)[noOfEmployees]->designation),stdin);
    (*employees)[noOfEmployees]->designation[strcspn((*employees)[noOfEmployees]->designation, "\n")] = '\0';

    if (strcmp((*employees)[noOfEmployees]->designation, "CEO") != 0)
    {
        int managerId;
        retryReportingManager:
        printf("Enter ID of reporting manager: ");
        scanf("%d", &managerId);
        Employee *emp = getEmployee(managerId, *employees);

        if (emp == NULL)
        {
            printf("Manager doesn't exist, please assign different reporting manager\n");
            goto retryReportingManager;
        }
        else{
            if(isReportingCycle(id,emp)){
                printf("Making circular reporting loop, please assign different reporting manager\n");
                goto retryReportingManager;
            }
            (*employees)[noOfEmployees]->reporting_manager = emp;

            emp->reports=realloc(emp->reports,(emp->reportsCount + 1 ) * sizeof(Employee *));
            emp->reports[emp->reportsCount]=(*employees)[noOfEmployees];

            emp->reportsCount+=1;

        }
    }

    noOfEmployees++;
    return 0;

}

Employee  *findBestSibling(Employee *employeePtr,Employee **employees){
    int reportCount=INT_MAX;

    Employee *bestSibling=NULL;

    for(int i=0; i < noOfEmployees; i++){
        if(employees[i] == employeePtr) continue;
        Employee emp=*employees[i];

        if(employeePtr->reporting_manager == emp.reporting_manager && reportCount > emp.reportsCount){
            reportCount=emp.reportsCount;
            bestSibling=employees[i];
        }
    }

    return bestSibling;
}

void shiftEmpPointers(Employee **employeePtr,Employee ***employees){
    int indx=0;

    while(indx != noOfEmployees && (*employees)[indx] != *employeePtr) indx++;

    free((*employeePtr)->reports);
    free(*employeePtr);
    *employeePtr=NULL;

    for(int i=indx; i < noOfEmployees-1; i++){
        (*employees)[i]=(*employees)[i+1];
    }

    *employees=realloc(*employees,(noOfEmployees-1) * sizeof(Employee *));
}

void removeFromReports(Employee *employeePtr,Employee ***reports,int reportsCount){

    int indx=0;

    while(indx != reportsCount && (*reports)[indx] != employeePtr) indx++;

    for(int i=indx; i < reportsCount-1; i++){
        (*reports)[i]=(*reports)[i+1];
    }

    *reports=realloc(*reports,(reportsCount - 1) * sizeof(Employee *));

}

int removeEmployee(Employee ***employees){
    printf("Enter ID of the employee: ");
    int id;
    scanf("%d",&id);

    Employee *employeePtr=getEmployee(id,*employees);

    if(employeePtr == NULL){
        printf("Employee with ID-%d doesn't exists, please try again\n",id);
        return 1;
    }

    if(strcmp(employeePtr->designation,"CEO") == 0){
        printf("CEO can't be remove\n");
        return 1;
    }

    Employee *manager=employeePtr->reporting_manager;
    if(manager != NULL){
        removeFromReports(employeePtr,&(manager->reports),manager->reportsCount);
        manager->reportsCount--;
    }

    if(employeePtr->reportsCount > 0){
        Employee *backupManager=findBestSibling(employeePtr,*employees);

        if(backupManager == NULL){
            backupManager=employeePtr->reporting_manager;
        }
        
        backupManager->reports=realloc(backupManager->reports,(backupManager->reportsCount + employeePtr->reportsCount) * sizeof(Employee *));

        int indx=backupManager->reportsCount;

        for(int i=0; i < employeePtr->reportsCount; i++){
            backupManager->reports[indx++]=employeePtr->reports[i];
            employeePtr->reports[i]->reporting_manager=backupManager;
        }

        backupManager->reportsCount+=employeePtr->reportsCount;
    }

    shiftEmpPointers(&employeePtr,employees);
    noOfEmployees--;

    return 0;
}

void printEmp(Employee **employees){
    for(int i=0; i < noOfEmployees; i++){
        printf("%s %d\n",employees[i]->name,employees[i]->reportsCount);
    }
}

void displayHierarchy(Employee *employee,int level,int *totalEmployees){
    if(employee == NULL) return;

    (*totalEmployees)++;

    int reportsCount=employee->reportsCount;

    for(int i=0; i < level; i++){
        printf("\t");
    }

    printf("%d - %s - %s\n",employee->id,employee->name,employee->designation);

    for(int i=0; i < reportsCount; i++){
        displayHierarchy(employee->reports[i],level + 1,totalEmployees);
    }
}

Employee *getCeo(Employee **employees){
    for(int i=0; i < noOfEmployees; i++){
        Employee currEmp=*employees[i];

        if(strcmp(currEmp.designation,"CEO") == 0) return employees[i];
    }

    return NULL;
}

void displayEmployeeHierarchy(Employee **employees){
    int id;

    printf("Enter the Employee ID to display their hierarchy: ");
    scanf("%d",&id);

    Employee *employee=getEmployee(id,employees);

    if(employee == NULL){
        printf("Employee not found!, please enter correct Employee ID\n\n");
        return;
    }

    int empUnderCount=0;

    displayHierarchy(employee,0,&empUnderCount);

    printf("\nTotal Employees Under %d: %d",employee->id,empUnderCount-1);

}

int main()
{

    int choice;
    Employee **employees = NULL;

    while (true)
    {

        printMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            if (addEmployee(&employees) == 0) printf("Employee added successfully\n");
            else printf("Failed please try again\n");
        }
        break;

        case 2:{
            if(removeEmployee(&employees) == 0) printf("Employee removed successfully\n");
            else printf("Employee removal failed!\n\n");
        }
        break;

        case 3:{
            displayEmployeeHierarchy(employees);
        }
        break;

        case 4:{
            Employee *ceo=getCeo(employees);
            int totalEmployees=0;
            if(ceo == NULL) printf("Error: CEO doesn't exists");
            else {
                printf("===== ORGANIZATION HIERARCHY =====\n\n");
                displayHierarchy(ceo,0,&totalEmployees);
                printf("\nTotal Employees: %d",totalEmployees);
            }
        }
        break;

        case 5:return 0;

        default:printf("Invalid choice. Please select a valid option.\n");
        break;

        }
    }
}