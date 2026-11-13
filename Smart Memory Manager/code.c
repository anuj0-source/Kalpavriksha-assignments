#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Block{
    int id;
    int start;
    int end;
    int size;
    int isFree;
}Block;

int blocksCount=0;
int idCount=0;

int failedAllocation=-1;
int failedAllocationSize=0;

void printMenu(){
    printf("\nMenu:\n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");
}

void removeBlock(Block **blocks,int pos){
    for(int i=pos; i < blocksCount - 1; i++){
        (*blocks)[i]=(*blocks)[i+1];
    }

    Block *temp=realloc(*blocks,(blocksCount - 1) * sizeof(Block));

    if(temp == NULL) return;

    *blocks=temp;
}

void shiftBlocks(Block **blocks,int indx){
    for(int i=blocksCount; i > indx; i--){
        (*blocks)[i]=(*blocks)[i-1];
    }
}

int insertBlock(Block **blocks,int pos,int requested_size){
    Block curr=*(*blocks + pos);

    int freeSize = curr.size;

    Block *temp = realloc(*blocks, (blocksCount + 1) * sizeof(Block));

    if (temp == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    *blocks = temp;

    if (freeSize - requested_size == 0)
    {
        (*blocks + pos)->isFree = 0;
        (*blocks + pos)->id = idCount++;
        
    }
    else
    {
        shiftBlocks(blocks, pos);

        (*blocks + pos)->id = idCount++;
        (*blocks + pos)->start = curr.start;
        (*blocks + pos)->end = curr.start + requested_size - 1;
        (*blocks + pos)->isFree = 0;
        (*blocks + pos)->size = requested_size;

        (*blocks + pos + 1)->start += requested_size;
        (*blocks + pos + 1)->size -= requested_size;

        blocksCount++;
    }

    return 0;
}

void firstFit(Block **blocks,int requested_size){
    int bestPos=-1;

    for(int i=0; i < blocksCount; i++){
        Block curr=*(*blocks + i);

        if(curr.isFree && curr.size >= requested_size){
            bestPos=i;
            break;
        }
    }

    if(bestPos == -1) {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        failedAllocation=1;
        failedAllocationSize=requested_size;
    }
    else{
        if(insertBlock(blocks,bestPos,requested_size) == 0) printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", (*blocks + bestPos)->id, (*blocks + bestPos)->start, requested_size);
    }
}

void bestFit(Block **blocks,int requested_size){
    int bestPos=-1;
    int bestSize=0;

    for(int i=0; i < blocksCount; i++){
        Block block=*(*blocks + i);

        if(block.isFree && block.size >= requested_size){
            if(bestPos == -1){
                bestPos=i;
                bestSize=block.size;
            }
            else{
                if(bestSize > block.size){
                    bestSize=block.size;
                    bestPos=i;
                }
            }
        }
    }

    if(bestPos == -1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        failedAllocation=1;
        failedAllocationSize=requested_size;
        return;
    }

    if(insertBlock(blocks,bestPos,requested_size) == 0) printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", (*blocks + bestPos)->id, (*blocks + bestPos)->start, requested_size);
}

void worstFit(Block **blocks,int requested_size){
    int bestPos=-1;
    int bestSize=0;

    for(int i=0; i < blocksCount; i++){
        Block block=*(*blocks + i);

        if(block.isFree && block.size >= requested_size){
            if(bestPos == -1){
                bestPos=i;
                bestSize=block.size;
            }
            else{
                if(bestSize < block.size){
                    bestSize=block.size;
                    bestPos=i;
                }
            }
        }
    }

    if(bestPos == -1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        failedAllocation=1;
        failedAllocationSize=requested_size;
        return;
    }

    if(insertBlock(blocks,bestPos,requested_size) == 0) printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", (*blocks + bestPos)->id, (*blocks + bestPos)->start, requested_size);
}

void allocate(Block **blocks,int strategy,int pool_size){

    int requested_size;
    printf("Requested size: ");
    scanf("%d",&requested_size);

    if(requested_size <= 0 || requested_size > pool_size){
        printf("Please enter allocation size between 1 to %d.\n",pool_size);
        return;
    }

    switch(strategy){
        case 1:
        firstFit(blocks,requested_size);
        break;

        case 2:
        bestFit(blocks,requested_size);
        break;

        case 3:
        worstFit(blocks,requested_size);
        break;

    }
}

int isExists(Block *blocks,int id){
    if(id <= 0) return -1;
    for(int i=0; i < blocksCount; i++){
        if((blocks + i)->id == id) return i;
    }

    return -1;
}

void freeBlock(Block **blocks,int pos){
    (*blocks + pos)->isFree=1;
    (*blocks + pos)->id=0;

    if(pos > 0 && (*blocks + pos - 1)->isFree){
        (*blocks + pos - 1)->end=(*blocks + pos)->end;
        (*blocks + pos - 1)->size+=(*blocks + pos)->size;
        removeBlock(blocks,pos);
        blocksCount--;
        pos--;
    }

    if(pos + 1 < blocksCount && (*blocks + pos + 1)->isFree){
        (*blocks + pos)->end=(*blocks + pos + 1)->end;
        (*blocks + pos)->size+=(*blocks + pos + 1)->size;
        removeBlock(blocks,pos + 1);
        blocksCount--;
    }
}

int release(Block **blocks){
    int id;
    printf("Allocation ID: ");
    scanf("%d",&id);

    int pos=isExists(*blocks,id);

    if(pos == -1){
        printf("ID doesn't found, Please enter correct allocation ID.\n");
        return 1;
    }

    if((*blocks + pos)->isFree){
        printf("Block is already free.\n");
        return 1;
    }

    freeBlock(blocks,pos);

    return 0;

}

void resizeBlock(Block **blocks,int pool_size,int strategy,char *pool){
    int id,newSize;
    printf("Allocation ID: ");
    if(scanf("%d",&id) != 1){
        printf("Please enter a valid allocation ID.\n");
        return;
    }

    int pos=isExists(*blocks,id);

    if(pos == -1 || (*blocks + pos)->isFree){
        printf("Allocation not found, Please enter correct Allocation ID,\n");
        return;
    }

    printf("Enter new size: ");
    if(scanf("%d",&newSize) != 1){
        printf("Please enter a valid size.\n");
        return;
    }

    if(newSize <= 0 || newSize > pool_size){
        printf("Please enter valid size.\n");
        return;
    }

    int oldSize=(*blocks + pos)->size;
    int oldStart=(*blocks + pos)->start;

    if(oldSize == newSize){
        printf("New size is same as old size, Please enter different size to resize the allocation.\n");
        return;
    }

    if(oldSize > newSize){
        int freedSize=oldSize - newSize;
        int oldEnd=(*blocks + pos)->end;

        if(pos + 1 < blocksCount && (*blocks + pos + 1)->isFree){
            (*blocks + pos + 1)->start-=freedSize;
            (*blocks + pos + 1)->size+=freedSize;
        }
        else{
            Block *temp=realloc(*blocks,(blocksCount + 1) * sizeof(Block));

            if(temp == NULL){
                printf("Resize failed: memory allocation failed.\n");
                return;
            }

            *blocks=temp;
            shiftBlocks(blocks,pos + 1);
            (*blocks + pos + 1)->id=0;
            (*blocks + pos + 1)->start=(*blocks + pos)->start + newSize;
            (*blocks + pos + 1)->end=oldEnd;
            (*blocks + pos + 1)->size=freedSize;
            (*blocks + pos + 1)->isFree=1;
            blocksCount++;
        }

        (*blocks + pos)->end=oldEnd - freedSize;
        (*blocks + pos)->size=newSize;
    }
    else{
        int extraSize=newSize - oldSize;
        int leftFreeSize=0;
        int rightFreeSize=0;

        if(pos > 0 && (*blocks + pos - 1)->isFree){
            leftFreeSize=(*blocks + pos - 1)->size;
        }

        if(pos + 1 < blocksCount && (*blocks + pos + 1)->isFree){
            rightFreeSize=(*blocks + pos + 1)->size;
        }

        if(extraSize > leftFreeSize + rightFreeSize){
            int newPos=-1;

            for(int i=0; i < blocksCount; i++){
                if(!(*blocks + i)->isFree || (*blocks + i)->size < newSize) continue;

                if(newPos == -1 ||
                   (strategy == 2 && (*blocks + i)->size < (*blocks + newPos)->size) ||
                   (strategy == 3 && (*blocks + i)->size > (*blocks + newPos)->size)){
                    newPos=i;
                }

                if(strategy == 1) break;
            }

            if(newPos == -1){
                printf("Resize failed: no contiguous free block is large enough.\n");
                return;
            }

            Block destination=*(*blocks + newPos);

            if(destination.size > newSize){
                Block *temp=realloc(*blocks,(blocksCount + 1) * sizeof(Block));

                if(temp == NULL){
                    printf("Resize failed: memory allocation failed.\n");
                    return;
                }

                *blocks=temp;
                shiftBlocks(blocks,newPos + 1);
                (*blocks + newPos + 1)->id=0;
                (*blocks + newPos + 1)->start=destination.start + newSize;
                (*blocks + newPos + 1)->end=destination.end;
                (*blocks + newPos + 1)->size=destination.size - newSize;
                (*blocks + newPos + 1)->isFree=1;
                blocksCount++;

                if(newPos < pos) pos++;
            }

            (*blocks + newPos)->id=id;
            (*blocks + newPos)->start=destination.start;
            (*blocks + newPos)->end=destination.start + newSize - 1;
            (*blocks + newPos)->size=newSize;
            (*blocks + newPos)->isFree=0;

            freeBlock(blocks,pos);
            pos=isExists(*blocks,id);
        }
        else{
            if(rightFreeSize > 0){
                int usedSize=extraSize < rightFreeSize ? extraSize : rightFreeSize;
                (*blocks + pos)->end+=usedSize;
                (*blocks + pos)->size+=usedSize;
                extraSize-=usedSize;

                if(usedSize == rightFreeSize){
                    removeBlock(blocks,pos + 1);
                    blocksCount--;
                }
                else{
                    (*blocks + pos + 1)->start+=usedSize;
                    (*blocks + pos + 1)->size-=usedSize;
                }
            }

            if(extraSize > 0){
                (*blocks + pos)->start-=extraSize;
                (*blocks + pos)->size+=extraSize;
                (*blocks + pos - 1)->end-=extraSize;
                (*blocks + pos - 1)->size-=extraSize;

                if((*blocks + pos - 1)->size == 0){
                    removeBlock(blocks,pos - 1);
                    blocksCount--;
                    pos--;
                }
            }
        }
    }

    if((*blocks + pos)->start != oldStart){
        memmove(pool + (*blocks + pos)->start,pool + oldStart,oldSize);
        printf("Allocation moved from %d to %d.\n",oldStart,(*blocks + pos)->start);
    }

    printf("Resize successful. ID: %d, Start: %d, Size: %d\n",id,(*blocks + pos)->start,newSize);

}

void printStrategy(int strategy){
    switch(strategy){
        case 1:printf("Strategy set to First Fit.\n");
        break;

        case 2:printf("Strategy set to Best Fit.\n");
        break;

        case 3:printf("Strategy set to Worst Fit.\n");
        break;
    }
}

void memoryMap(Block *blocks,int pool_size){
    printf("Memory Map (%d bytes):\n",pool_size);

    for(int i=0; i < blocksCount; i++){
        Block block=*(blocks + i);

        if(block.isFree){
            printf("[Free: %d bytes, Start: %d]\n",block.size,block.start);
        }
        else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n",block.id,block.size,block.start);
        }
    }
}

void memoryStatistics(Block *blocks,int pool_size){
    printf("Total memory: %d bytes\n",pool_size);

    int allocatedCount=0;
    int freeBlocks=0;
    int maxFreeSize=0;

    for(int i=0; i < blocksCount; i++){
        Block block=*(blocks + i);

        if(!block.isFree) allocatedCount+=block.size;
        else{
            freeBlocks++;

            if(block.size > maxFreeSize) maxFreeSize=block.size;
        }

    }

    printf("Allocated: %d bytes\n",allocatedCount);
    printf("Free: %d bytes\n",pool_size - allocatedCount);
    printf("Allocated blocks: %d\n",blocksCount - freeBlocks);
    printf("Free blocks: %d\n",freeBlocks);
    printf("Largest free block: %d bytes\n",maxFreeSize);

    if(failedAllocation == 1 && failedAllocationSize <= pool_size - allocatedCount && maxFreeSize < failedAllocationSize){
        printf("Note: The request for %d bytes failed even though %d bytes are free, because the free memory is fragmented.",failedAllocationSize,pool_size - allocatedCount);
    }

    failedAllocation=-1;
    failedAllocationSize=0;
}

void changeStrategy(int *currStrategy){
    int new;
    printf("Select new allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");
    scanf("%d",&new);

    if(new == *currStrategy){
        printf("Selected option already set.\n");
        return;
    }

    if(new < 1 || new > 3){
        printf("Allocation strategy change failed, Please choose correct strategy.\n");
        return;
    }

    *currStrategy=new;

    if(*currStrategy == 1) printf("Strategy set to First Fit\n");
    if(*currStrategy == 2) printf("Strategy set to Best Fit\n");
    if(*currStrategy == 3) printf("Strategy set to Worst Fit\n");

}

int main(){

    int pool_size,allocation_strategy;

    printf("\nEnter memory pool size: ");

    scanf("%d",&pool_size);

    if(pool_size <= 0 || pool_size > 1000000){
        printf("The memory pool size must be between 1 and 1,000,000 bytes.\n");
        return 0;
    }

    printf("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");
    scanf("%d",&allocation_strategy);

    if(allocation_strategy < 1 || allocation_strategy > 3){
        printf("Please choose correct strategy.\n");
        return 0;
    }

    printStrategy(allocation_strategy);

    char *pool=malloc(pool_size);
    Block *blocks=malloc(1 * sizeof(Block));
    
    if(pool == NULL || blocks == NULL){
        printf("Memory allocation failed, please try again.\n");
        return 0;
    }

    blocks->id=0;
    blocks->start=0;
    blocks->end=pool_size - 1;
    blocks->isFree=1;
    blocks->size=pool_size;

    blocksCount++;
    idCount++;

    while(1){
        int choice;

        printMenu();
        scanf("%d",&choice);

        switch(choice){
            case 1:{
                printf("\nChoice 1:\n");
                allocate(&blocks,allocation_strategy,pool_size);
                break;
            }

            case 2:{
                printf("\nChoice: 2\n");
                if(release(&blocks)) printf("Allocation release failed.\n");
                else printf("Allocation released\n");
                break;
            }

            case 3:
            printf("\nChoice: 3\n");
            resizeBlock(&blocks,pool_size,allocation_strategy,pool);
            break;

            case 4:printf("\nChoice: 4\n");
            memoryMap(blocks,pool_size);
            break;

            case 5:printf("\nChoice: 5\n");
            memoryStatistics(blocks,pool_size);
            break;

            case 6:printf("\nChoice: 6\n");
            changeStrategy(&allocation_strategy);
            break;

            case 7:printf("\nChoice: 7\n");
            free(pool);
            free(blocks);
            printf("Program exitted.\n");
            return 0;

            default:printf("Please choose correct option.\n");
            break;

        }
    }
    return 0;
}
