#include<stdio.h>
#include<string.h>
#include<stdbool.h>

int getPrecedence(char operator){
    if(operator == '*' || operator == '/') return 2;
    else if(operator == '+' || operator == '-') return 1;
    else return 0;
}

bool isValid(char ch){
    if(ch >= 48 && ch <= 57) return true;
    else if(ch == '+' || ch == '-' || ch == '*' || ch == '/') return true;
    else return false;
}

int calculate(char operators[],int operands[],int *topOperator,int *topOperand,bool *err,char errorMsg[]){

    if(*topOperand < 1 || *topOperator < 0){
        *err=true;
        strcpy(errorMsg,"Error: Invalid expression");
        return -1;
    }
    int b=operands[(*topOperand)--];
    int a=operands[(*topOperand)--];

    char operator=operators[(*topOperator)--];

    int result=0;

    switch(operator){ 
        case '+':
        result=a + b;
        break;

        case '-':
        result=a - b;
        break;

        case '*':
        result=a * b;
        break;
        
        case '/':
        if(b == 0) {
            strcpy(errorMsg,"Error: Division by 0");
            *err=true;
        }
        else result=a / b;
        break;
    }

    return result;

}

int main(){
    int operands[1000];
    char operators[1000];

    char expression[1000];

    printf("Enter expression: ");
    fgets(expression, sizeof(expression), stdin);
    int indx=0;
    int topOperand=-1;
    int topOperator=-1;

    bool err=false;
    char errorMsg[100];

    while(expression[indx] != '\0'){
        char ch=expression[indx++];

        if(ch == ' ' || ch == '\n'){
            continue;
        }

        if(!isValid(ch)){
            err=true;
            strcpy(errorMsg,"Error: Invalid expression");
            break;
        }

        if(ch >= 48 && ch <= 57){
            int num=ch - '0';

            while (expression[indx] >= '0' && expression[indx] <= '9')
            {
                num=num * 10 + (expression[indx] - '0');
                indx++;
            }

            operands[++topOperand]=num;
            
        }
        else{
            while(topOperator != -1 && getPrecedence(operators[topOperator]) >= getPrecedence(ch)){
                int result=calculate(operators,operands,&topOperator,&topOperand,&err,errorMsg);
                operands[++topOperand]=result;
                if(err){
                    break;
                }
            }

            operators[++topOperator]=ch;

        }

    }

    while(!err && topOperator >= 0){

        int result=calculate(operators,operands,&topOperator,&topOperand,&err,errorMsg);
        operands[++topOperand]=result;

        if(err){
            break;
        }
    }


    if(err){
        printf("%s",errorMsg);
    }
    else{
        printf("Result of expression: %d",operands[topOperand]);
    }

}