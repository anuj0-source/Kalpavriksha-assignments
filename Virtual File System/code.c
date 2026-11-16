#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024

typedef struct FreeBlock{
    unsigned char *index;
    struct FreeBlock *prev;
    struct FreeBlock *next;
}FreeBlock;

typedef struct FileNode{
    char name[51];
    int isDir;
    int bytesOccupied;
    int blocksOccupied;
    unsigned char **blockPointers;
    struct FileNode *next;
    struct FileNode *childHead;
    struct FileNode *childTail;
    struct FileNode *parent;
}FileNode;

typedef struct DiskManager{
    int total_blocks;
    int free_blocks;
    FreeBlock *head;
    FreeBlock *tail;
}DiskManager;

DiskManager InitializeDiskManager(){
    DiskManager diskManager;
    diskManager.free_blocks=NUM_BLOCKS;
    diskManager.total_blocks=NUM_BLOCKS;
    diskManager.head=diskManager.tail=NULL;

    return diskManager;
}

FreeBlock *createFreeBlockNode(unsigned char *index){
    FreeBlock *block=malloc(sizeof(FreeBlock));

    if(block == NULL){
        printf("Error: memory allocation failed for FreeBlock node.\n");
        return block;
    }

    block->index=index;
    block->prev=block->next=NULL;

    return block;
}

FileNode *createFileNode(char name[],int isDir){
    FileNode *node=calloc(1,sizeof(FileNode));

    if(node == NULL){
        printf("Error: memory allocation failed for FileNode.\n");
        return node;
    }

    strcpy(node->name,name);
    node->isDir=isDir;

    return node;
}

FileNode *findDir(char name[],FileNode *childHead){
    if(childHead == NULL) return NULL;

    FileNode *temp=childHead;

    do{
        if(strcmp(temp->name,name) == 0 && temp->isDir) return temp;
        temp=temp->next;
    }while(temp != childHead);

    return NULL;
}

FileNode *findFile(char fileName[],FileNode *childHead){
    if(childHead == NULL) return NULL;

    FileNode *temp=childHead;

    do{
        if(strcmp(fileName,temp->name) == 0 && !temp->isDir) return temp;
        temp=temp->next;
    }while(temp != childHead);

    return NULL;
}

void listDir(FileNode *cwd){
    if(cwd == NULL) return;

    if(cwd->childHead == NULL){
        printf("(empty)\n");
        return;
    }

    FileNode *temp=cwd->childHead;

    do{
        printf("%s\n",temp->name);
        temp=temp->next;
    }while(temp != cwd->childHead);
}

int write(char input[],int indx,FileNode *cwd,DiskManager *diskManager){
    if(cwd == NULL) return 0;

    int j=0;

    char fileName[51]={'\0'};

    while(input[indx] != ' ' && input[indx] != '\0' && j < 50){
        fileName[j++]=input[indx++];
    }

    if(j == 50 && input[indx] != ' ' && input[indx] != '\0'){
        printf("Please enter a valid file name.\n");
        return 0;
    }
    
    while(input[indx] == ' ') indx++;

    if(input[indx] == '\0'){
        printf("Please enter content after filename to write into files.\n");
        return 0;
    }

    FileNode *filePtr=findFile(fileName,cwd->childHead);

    if(filePtr == NULL){
        printf("File named '%s' not found.\n",fileName);
        return 0;
    }

    if(input[indx] == '"'){
        indx++;
        int end=strcspn(input,"\0");
        
        int temp=indx;
        while(input[temp] != '"' && input[temp] != '\0') temp++;

        if(input[temp] == '\0'){
            printf("Please close the content with double quote.\n");
            return 0;
        }
        temp++;
        while(input[temp] == ' ') temp++;

        if(input[temp] != '\0'){
            printf("Please enter a valid command.\n");
            return 0;
        }

        int size=end - indx - 1;

        int blocksReq=(int) ceil((double) size / (BLOCK_SIZE - 1));

        if(filePtr->blocksOccupied){
            for (int i = 0; i < filePtr->blocksOccupied; i++)
            {
                FreeBlock *freeBlock = createFreeBlockNode(*(filePtr->blockPointers + i));

                if (diskManager->tail == NULL)
                {
                    diskManager->head = diskManager->tail = freeBlock;
                }
                else
                {
                    diskManager->tail->next = freeBlock;
                    freeBlock->prev = diskManager->tail;
                    diskManager->tail = freeBlock;
                }

                diskManager->free_blocks++;
            }
        }

        if(blocksReq > diskManager->free_blocks){
            printf("Not enough free blocks.\n");
            return 0;
        }

        unsigned char **tempPtr=realloc(filePtr->blockPointers,blocksReq * sizeof(unsigned char *));

        if(tempPtr == NULL){
            printf("Memory allocation failed.\n");
            return 0;
        }

        filePtr->blockPointers=tempPtr;

        for(int i=0; i < blocksReq; i++){
            FreeBlock *freePtr=diskManager->head;
            *(filePtr->blockPointers + i)=freePtr->index;
            freePtr->next->prev=NULL;
            diskManager->head=freePtr->next;
            free(freePtr);
            diskManager->free_blocks--;

            int j=0;

            while(input[indx] != '"' && j < BLOCK_SIZE){
                *(*(filePtr->blockPointers + i) + j)=input[indx++];
                j++;
            }

            *(*(filePtr->blockPointers + i) + j)='\0';
        }

        filePtr->blocksOccupied=blocksReq;
        filePtr->bytesOccupied=size;

        printf("Data written successfully (size = %d bytes).\n",size);

        return 1;

    }else{
        printf("Write contents under double quotes.\n");
        return 0;
    }
}

void read(char input[],int indx,FileNode *cwd){
    if(cwd == NULL) return;

    int j=0;

    char fileName[51]={'\0'};

    while(input[indx] != ' ' && input[indx] != '\0' && j < 50){
        fileName[j++]=input[indx++];
    }

    if(j == 50 && input[indx] != ' ' && input[indx] != '\0'){
        printf("Please enter a valid file name.\n");
        return;
    }
    
    while(input[indx] == ' ') indx++;

    if(input[indx] != '\0'){
        printf("Please enter a valid command.\n");
        return;
    }

    FileNode *filePtr=findFile(fileName,cwd->childHead);

    if(filePtr == NULL){
        printf("File named '%s' not found.\n",fileName);
        return;
    }

    for(int i=0; i < filePtr->blocksOccupied; i++){
        printf("%s\n",*(filePtr->blockPointers + i));
    }

}

void mkdir(char input[],int i,FileNode *cwd){
    char dirName[52] = {'\0'};
    int j = 0;

    while (input[i] != '\0' && input[i] != ' ')
    {
        dirName[j++] = input[i++];
    }

    if (j > 51)
    {
        printf("Directories name should be less than or equal to 51 characters.\n");
        return;
    }

    while (input[i] == ' ' && input[i] != '\0')
        i++;

    if (input[i] != '\0')
    {
        printf("Please enter directory name without whitespaces.\n");
        return;
    }

    FileNode *childPtr = cwd->childHead;

    if (findDir(dirName, childPtr))
    {
        printf("Directory named %s already exists.\n", dirName);
        return;
    }

    FileNode *newDir = createFileNode(dirName, 1);
    newDir->parent = cwd;

    if (childPtr == NULL)
    {
        cwd->childHead = cwd->childTail = newDir;
        newDir->next = newDir;
    }
    else
    {
        newDir->next = childPtr;
        cwd->childHead = newDir;
        cwd->childTail->next = cwd->childHead;
    }

    printf("Directory '%s' created successfully.\n", dirName);
}

void changeDirectory(char input[],int i,FileNode **cwd){
    char postCommand[51] = {'\0'};
    int j = 0;

    while (input[i] != '\0' && input[i] != ' ')
    {
        postCommand[j++] = input[i++];
    }

    while (input[i] == ' ' && input[i] != '\0')
        i++;

    if (input[i] != '\0' || j >= 50)
    {
        printf("Please enter a valid directory name.\n");
        return;
    }

    if (strcmp(postCommand, "..") == 0)
    {
        if ((*cwd)->parent != NULL)
            *cwd = (*cwd)->parent;
    }
    else
    {
        FileNode *childHead = (*cwd)->childHead;
        FileNode *dirPtr = findDir(postCommand, childHead);

        if (dirPtr == NULL)
        {
            printf("Cannot find /%s because it doesn't exist.\n", postCommand);
            return;
        }

        *cwd = dirPtr;
    }

    if((*cwd)->parent) printf("Moved to /%s\n", (*cwd)->name);
    else printf("Moved to %s\n", (*cwd)->name);
}

void create(char input[],int i,FileNode *cwd){
    char fileName[51] = {'\0'};
    int j = 0;

    while (input[i] != '\0' && input[i] != ' ')
    {
        fileName[j++] = input[i++];
    }

    if (j >= 50)
    {
        printf("Files name should be less than or equal to 50 characters.\n");
        return;
    }

    while (input[i] == ' ' && input[i] != '\0')
        i++;

    if (input[i] != '\0')
    {
        printf("Please enter file name without whitespaces.\n");
        return;
    }

    FileNode *childPtr = cwd->childHead;

    if (findFile(fileName, childPtr))
    {
        printf("File named %s already exists.\n", fileName);
        return;
    }

    FileNode *newFile = createFileNode(fileName, 0);
    newFile->parent = cwd;

    if (childPtr == NULL)
    {
        cwd->childHead = cwd->childTail = newFile;
        newFile->next = newFile;
    }
    else
    {
        newFile->next = childPtr;
        cwd->childHead = newFile;
        cwd->childTail->next = cwd->childHead;
    }

    printf("File '%s' created successfully.\n", fileName);
}

void presentWorkingDirectory(FileNode *cwd){
    if(cwd->parent == NULL) return;

    presentWorkingDirectory(cwd->parent);

    printf("/%s",cwd->name);

}

void df(DiskManager *diskManager){
    printf("Total Blocks: %d\n",NUM_BLOCKS);
    int usedBlocks=diskManager->total_blocks - diskManager->free_blocks;
    printf("Used Blocks: %d\n",usedBlocks);
    printf("Free Blocks: %d\n",diskManager->free_blocks);
    double diskUsage=(double) usedBlocks / (double) diskManager->total_blocks * 100;
    printf("Disk Usage: %.2lf%%\n",diskUsage);
}

void delete(char input[],int indx,FileNode *cwd,DiskManager *diskManager){
    if(cwd == NULL) return;

    int j=0;

    char fileName[51]={'\0'};

    while(input[indx] != ' ' && input[indx] != '\0' && j < 50){
        fileName[j++]=input[indx++];
    }

    if(j == 50 && input[indx] != ' ' && input[indx] != '\0'){
        printf("Please enter a valid file name.\n");
        return;
    }
    
    while(input[indx] == ' ') indx++;

    if(input[indx] != '\0'){
        printf("Please enter a valid command.\n");
        return;
    }

    FileNode *filePtr=findFile(fileName,cwd->childHead);

    if(filePtr == NULL){
        printf("File named '%s' not found.\n",fileName);
        return;
    }

    for(int i=0; i < filePtr->blocksOccupied; i++){
        FreeBlock *freeBlock=createFreeBlockNode(*(filePtr->blockPointers + i));

        if(diskManager->tail == NULL){
            diskManager->head=diskManager->tail=freeBlock;
        }
        else{
            diskManager->tail->next=freeBlock;
            freeBlock->prev=diskManager->tail;
            diskManager->tail=freeBlock;
        }

        diskManager->free_blocks++;
    }

    free(filePtr->blockPointers);

    FileNode *temp=cwd->childHead;
    FileNode *prev=NULL;

    while(temp != filePtr){
        prev=temp;
        temp=temp->next;
    }

    if(temp->next == temp) cwd->childHead=cwd->childTail=NULL;

    else if(cwd->childHead == temp){
        cwd->childTail->next=temp->next;
        cwd->childHead=temp->next;

    }else{
        prev->next=temp->next;
        if(temp == cwd->childTail) cwd->childTail=prev;
    }

    free(temp);

    printf("'%s' deleted successfully.\n",fileName);
}

void rmdir(char input[],int indx,FileNode *cwd){
    char dirName[51]={'\0'};

    int j=0;

    while(input[indx] != ' ' && input[indx] != '\0' && j < 50){
        dirName[j++]=input[indx++];
    }

    if(j >= 50 && input[indx] != ' ' && input[indx] != '\0'){
        printf("Please enter a valid directory name to delete.\n");
        return;
    }

    while(input[indx] == ' ') indx++;

    if(input[indx] != '\0'){
        printf("Please enter a valid command.\n");
        return;
    }

    FileNode *dir=findDir(dirName,cwd->childHead);

    if(dir == NULL){
        printf("Directory named '%s' not found.\n",dirName);
        return;
    }

    if(dir->childHead){
        printf("Directory is not empty and cannot be deleted.\n");
        return;
    }

    FileNode *temp=cwd->childHead;
    FileNode *prev=NULL;

    while(temp != dir){
        prev=temp;
        temp=temp->next;
    }

    if(temp->next == temp) cwd->childHead=cwd->childTail=NULL;

    else if(cwd->childHead == temp){
        cwd->childTail->next=temp->next;
        cwd->childHead=temp->next;

    }else{
        prev->next=temp->next;
        if(temp == cwd->childTail) cwd->childTail=prev;
    }

    free(temp);

    printf("Directory '%s' removed successfully.\n",dirName);
}

void freeAllFiles(FileNode *cwd){
    if(cwd->childHead == NULL) return;

    FileNode *temp=cwd->childHead;

    do{
        if(temp->isDir) freeAllFiles(temp);
        else free(temp->blockPointers);

        FileNode *removing=temp;
        temp=temp->next;
        free(removing);
    }while(temp != cwd->childHead);

}

void freeAllBlocks(DiskManager *diskManager){

    FreeBlock *removing=diskManager->head;

    while(removing){
        free(removing->index);
        FreeBlock *temp=removing;
        removing=removing->next;

        free(temp);
    }
}

int main(){
    unsigned char **disk=malloc(NUM_BLOCKS * sizeof(unsigned char *));
    DiskManager diskManager=InitializeDiskManager();

    for(int i=0; i < NUM_BLOCKS; i++){
        *(disk + i)=calloc(512,sizeof(unsigned char));

        FreeBlock *freeBlockNode=createFreeBlockNode(*(disk + i));

        if(diskManager.head == NULL){
            diskManager.head=diskManager.tail=freeBlockNode;
        }else{
            freeBlockNode->prev=diskManager.tail;
            diskManager.tail->next=freeBlockNode;
            diskManager.tail=freeBlockNode;
        }
    }

    FileNode *cwd=createFileNode("/",1);

    printf("\n$ ./vfs\nCompact VFS - ready. Type 'exit' to quit.\n");

    while(1){
        char input[6000]={'\0'};
        printf("%s > ",cwd->name);
        fgets(input,1999,stdin);
        input[strcspn(input,"\n")]='\0';

        char command[100]={'\0'};
        int i=0;
        int j=0;

        while(input[i] == ' ' && input[i] != '\0') i++;

        while(input[i] != ' ' && input[i] != '\0'){
            command[j++]=input[i++];
        }
        while(input[i] == ' ' && input[i] != '\0') i++;

        //mkdir
        if(strcmp(command,"mkdir") == 0){
            if(input[i] == '\0'){
                printf("Please enter directory name.\n");
                continue;
            }
            
            mkdir(input,i,cwd);
        }
    

        //cd
        else if(strcmp(command,"cd") == 0){
            changeDirectory(input,i,&cwd);
        }

        //create
        else if(strcmp(command,"create") == 0){
            if(input[i] == '\0'){
                printf("Please enter file name.\n");
                continue;
            }

            create(input,i,cwd);
        }

        //ls
        else if(strcmp(command,"ls") == 0){
            if(input[i] != '\0'){
                printf("Please enter valid command.\n");
                continue;
            }

            listDir(cwd);
        }

        //write
        else if(strcmp(command,"write") == 0){
            if(input[i] == '\0'){
                printf("Please enter file name and content to write into the file.\n");
                continue;
            }

            if(diskManager.free_blocks == 0){
                printf("Memory full, no free block available.\n");
                continue;
            }

            write(input,i,cwd,&diskManager);
        }

        // read
        else if(strcmp(command,"read") == 0){
            if(input[i] == '\0'){
                printf("Please enter file name to read the file.\n");
                continue;
            }

            read(input,i,cwd);
        }

        //df
        else if(strcmp(command,"df") == 0){
            if(input[i] != '\0'){
                printf("Please enter a valid command.\n");
                continue;
            }

            df(&diskManager);
        }

        // pwd
        else if(strcmp(command,"pwd") == 0){
            if(input[i] != '\0'){
                printf("Please enter a valid command.\n");
                continue;
            }

            if(cwd->parent == NULL) printf("%s",cwd->name);
            else presentWorkingDirectory(cwd);
            printf("\n");
        }

        //delete
        else if(strcmp(command,"delete") == 0){
            if(input[i] == '\0'){
                printf("Please enter the file name to delete the file.\n");
                continue;
            }

            delete(input,i,cwd,&diskManager);
        }

        //rmdir
        else if(strcmp(command,"rmdir") == 0){
            if(input[i] == '\0'){
                printf("Please enter the file name to delete the file.\n");
                continue;
            }

            rmdir(input,i,cwd);
        }

        //exit
        else if(strcmp(command,"exit") == 0){
            if(input[i] != '\0'){
                printf("Please enter a valid command.\n");
                continue;
            }

            freeAllFiles(cwd);
            freeAllBlocks(&diskManager);
            free(disk);

            printf("Memory released. Exiting program...\n");
            return 0;
        }
        
        else{
            printf("Please enter a valid command.\n");
        }
    }

}