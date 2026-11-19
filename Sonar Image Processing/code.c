#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void assignRandomVal(int n,int matrix[n][n]){

    for(int i=0; i < n; i++){
        for(int j=0; j < n; j++){
            *(*(matrix + i) + j)=rand() % 256;
        }
    }

}

void printMatrix(int n,int matrix[n][n]){
    for(int i=0; i < n; i++){
        for(int j=0; j < n; j++){
            printf("%d ",*(*(matrix + i) + j));
        }

        printf("\n");
    }
}

void rotate(int n,int matrix[n][n]){

    n-=1;

    for(int i=0; i <= n/2; i++){
        for(int j=i; j <= n-i-1; j++){

            int curr=*(*(matrix + j) + i);

            int down=*(*(matrix + (n-i)) + j);

            int right=*(*(matrix + i)+(n-j));

            int rDown=*(*(matrix + (n - j)) + (n-i));

            int t1=curr;

            *(*(matrix + j) + i)=down;

            int t2=right;

            *(*(matrix + i)+(n-j))=t1;

            t1=rDown;

            *(*(matrix + (n - j)) + (n-i))=t2;

            *(*(matrix + (n-i)) + j)=t1;
            
        }
    }

}

void smooth(int n,int matrix[n][n]){

    int helper[n];
    
    for(int i=0; i < n; i++){
        int prev=0;

        for(int j=0; j < n; j++){

            int lUp, up, rUp, l, r, lDown, down, rDown;
            lUp=up=rUp=l=r=lDown=down=rDown=0;

            int count=0;

            if(i - 1 >= 0){
                count++;

                up=*(helper + j);

                if(j - 1 >= 0){
                    lUp=*(helper + (j - 1));
                    
                    count++;
                }

                if(j + 1 < n){
                    rUp=*(helper + (j + 1));
                    count++;
                }
            }

            if(j - 1 >= 0){
                l=*(*(matrix + i) + (j - 1));
                *(*(matrix + i) + (j - 1))=prev;
                *(helper + (j-1))=l;
                count++;
            }
            if(j + 1 < n){
                r=*(*(matrix + i) + (j + 1));
                count++;
            }

            if(i + 1 < n){
                down=*(*(matrix + (i+1)) + j);
                count++;

                if(j - 1 >= 0){
                    lDown=*(*(matrix + (i + 1)) + (j - 1));
                    count++;
                }

                if(j + 1 < n){
                    rDown=*(*(matrix + (i + 1)) + (j + 1));
                    count++;
                }
            }

            int curr=*(*(matrix + i) + j);
            count++;

            prev=(lUp + up + rUp + l + r + lDown + down + rDown + curr) / count;

        }
        *(helper + (n-1))=*(*(matrix + i) + (n-1));
        *(*(matrix + i) + (n-1))=prev;
    }
}

int main(){

    int n;

    do{
        printf("Enter matrix size (2-10): ");
        if(scanf("%d",&n) != 1){
            printf("Please enter a valid Integer.\n");
            return 0;
        }

        if(n < 2 || n > 10){
            printf("Please enter size between 2 to 10.\n");
        }

        
    }while(n < 2 || n > 10);
    

    int matrix[n][n];

    srand(time(NULL));
    assignRandomVal(n,matrix);

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(n,matrix);

    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    rotate(n,matrix);
    printMatrix(n,matrix);

    printf("\nMatrix after Applying 3X3 Smoothing Filter:\n");
    smooth(n,matrix);
    printMatrix(n,matrix);

    return 0;
}