#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "tela.h"

//Global variable that salva how many servers the client wanna simulate
int nServers;

//struct to salva the user variables
typedef struct tno{
    int key;
    short especial; //1: Usuário especial (gestante, idoso etc) 0: Não especial
    //salva the time when the user entered the queue
    time_t tIn;
    struct tno *next;
}no;

//struct to configure the users queue
struct userSet{
    //salva the time to input a new user
    int tToInput;
    //Max number of users
    //int maxUser;

};

//struct to salva the servers variables
struct t_server{
    //Time to output a user
    int tToOutput;
    //Number of users at the queue
    int inQueue;
    //Index of the queue
    int index;
    //Max time waiting
    time_t maxWaiting;
    //Queue
    no *begin;
    no *end;
    //Users Settings
    struct userSet users;
};

char leitura() {				// Leitura de 1 caracter nao-bloqueante
    if (kbhit()) {				// retorna true se alguma tecla foi pressionada
            return getch();
    }
    return '\0';
}

//procudure to start all servers
void setAllServers(struct t_server servers[]){
    int i;
    for (i=0;i<nServers;i++){
        servers[i].begin=NULL;
        servers[i].end=NULL;
        servers[i].inQueue=0;
        servers[i].maxWaiting=0;
    }
}


//procedure that insert a user
int insert(struct t_server servers[],int nServer, int newKey, char ehEspecial){
    time_t now;  //now: salva the actual time;
    no *newOne;
    time(&now);
    newOne=(no *) malloc(sizeof(no));
    //ERROR HEAPOVERFLOW
    if(newOne==NULL) return(-1);
    //if the Queue is empty

    if (servers[nServer].begin==NULL){
        servers[nServer].end=servers[nServer].begin=newOne;
        servers[nServer].index=0;
        (servers[nServer].begin)->key=newKey;
        (servers[nServer].begin)->next=NULL;
        (servers[nServer].begin)->tIn=now;
        if(ehEspecial)
            (servers[nServer].begin)->especial=1;
        else
            (servers[nServer].begin)->especial=0;

    }
    //if the Queue is not empty
    else{
        if(ehEspecial){
            no *aux = servers[nServer].begin;
            servers[nServer].begin=newOne;
            servers[nServer].index++;
            (servers[nServer].begin)->key=newKey;
            (servers[nServer].begin)->next=aux;
            (servers[nServer].begin)->tIn=now;
            (servers[nServer].begin)->especial=1;
        }
        else{
            (servers[nServer].end)->next=newOne;
            servers[nServer].end=(servers[nServer].end->next);
            servers[nServer].index++;
            servers[nServer].end->key=newKey;
            servers[nServer].end->next=NULL;
            servers[nServer].end->tIn=now;
            (servers[nServer].end)->especial=0;
        }
    }
        //printf("\nKey= %d, Input time= %d",servers[nServer].end->key, servers[nServer].end->tIn);
    return(1);
}

//procedure that delete a user
void del(struct t_server servers[], int nServer){
    time_t now;
    no *aux;
    time(&now);

    if(servers[nServer].begin!=NULL){
        aux=servers[nServer].begin;

        //Update maxWaiting
        if(difftime(now, servers[nServer].begin->tIn) > servers[nServer].maxWaiting)
            servers[nServer].maxWaiting = difftime(now, servers[nServer].begin->tIn);

        servers[nServer].begin=(servers[nServer].begin->next);
        free(aux);
        servers[nServer].index--;

    }
}

//procedure that prints all the queue
void printAll(struct t_server servers[],int lastPrintLine){
    int i;
    no *actual;

    for (i=0; i<nServers; i++){
        printf("\n\nServer %d:", i+1);
        printf("\n  Queue: ");
        actual=servers[i].begin;
        if(actual==NULL){
            printf("Empty queue!");
        }
        else{
            while(actual!=NULL){
                printf("%d, ",actual->key);
                actual=actual->next;
            }
            printf("\n  Max time waiting: %d\n  Size of the final queue: %d", servers[i].maxWaiting, servers[i].index+1);
        }
    }
    printf("\n");
}

//procedure that simulate the servers
int simulate(struct t_server servers[],int nServer,int *tToFinish, int *lastLine){
    time_t begin; //begin: salva the begin of the function
    time_t now; //now: salva the actual time;
    time_t lastIn, lastPrint; //lastIn: salva the time of the last user input
    no* aux=NULL; //aux: Print the queue
    time_t lastOutput=NULL;
    int newKey=1,lastPrintLine;
    char ehEspecial;

    time(&begin);
    time(&lastIn);
    no *newUser=NULL;
    gotoxy(0,5);
    printf("SIMULATING SERVER %d", nServer+1);
    do{
        time(&now);

        //Update the clock
		struct tm * timeinfo = localtime (&now);
		gotoxy(0,40);
		printf ("%2d:%2d:%2d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

        ehEspecial = leitura();  //Leitura do teclado
        if (ehEspecial){         //Caso o usuario digite algo
            if(insert(servers, nServer, newKey, ehEspecial)!=1){//ERROR STACKOVERFLOW
                return(-1);
            }
            newKey++;
            ehEspecial=NULL;
        }

        if(difftime(now, lastIn)>=servers[nServer].users.tToInput){
            time(&lastIn);
            if(insert(servers, nServer, newKey, ehEspecial)!=1){//ERROR STACKOVERFLOW
                return(-1);
            }
            newKey++;
        }
        if((int)difftime(now, begin)%servers[nServer].tToOutput==0 && now!=lastOutput){
            time(&lastOutput);
            del(servers,nServer);
        }

        //Print the queue
        if(now!=lastPrint){
            time(&lastPrint);
            aux=servers[nServer].begin;
            gotoxy(*lastLine+2,0);
            printf("Server %d: ",nServer+1);
            printf("\nTime: %d",(int)difftime(now,begin));
            printf("\nQueue:");
            lastPrintLine=*lastLine+4;
            while(aux!=NULL){
                lastPrintLine++;
                gotoxy(lastPrintLine, 0);
                struct tm * timeinfo2 = localtime (&aux->tIn);
                printf("Key: %d, Input time: %2dhrs %2dmins %2dsecs, Waiting time: %d sec",aux->key,timeinfo2->tm_hour, timeinfo2->tm_min, timeinfo2->tm_sec,(int)difftime(now,aux->tIn));
                if(aux->especial){
                    printf(", [ESPECIAL]");
                }
                else{
                    printf(", [GENERICO]");
                }
                aux=aux->next;
            }
        }
    }while((int)difftime(now,begin)<(*tToFinish));

    *lastLine=lastPrintLine;
    *tToFinish=(int)difftime(now,begin);
    return(1);
}


int main(){
    //User Config
    int opt,nUsers,tToFinish,i,lastLine=0;

    textcolor(VERDE_ESCURO);
    do{
        //informations
        system("cls");
        printf("Welcome to ServerQueueSimulator 1.6\n");

        do{
            printf("\nServers[1 a 100]: ");
            scanf("%d", &nServers);
        }while(nServers<1 || nServers>100);
        struct t_server servers[nServers];

        for(i=0;i<nServers;i++){
            //printf("How many users?");
            //scanf("%d",&servers[0].users.maxUser);

            printf("\n------------------- SERVER %d -------------------\n", i+1);
            printf("Time to input which user(sec): ");
            scanf("%d",&servers[i].users.tToInput);

            printf("Time to output a user(sec): ");
            scanf("%d",&servers[i].tToOutput);
        }
        printf("\n\nTime to finish the simulation(sec): ");
        scanf("%d",&tToFinish);

        setAllServers(servers);
        system("cls");

        //simulating
        for(i=0;i<nServers;i++){
            if(simulate(servers,i,&tToFinish,&lastLine)!=1){
                printf("\n!!!ERROR!!!\n");
                return(-1);
            }
        }

        //prints
        system("cls");
        printf("\n\n------------------- DETAILS -------------------");
        printAll(servers, lastLine);
        printf("\n-----------------------------------------------");

       //end
        printf("\nDo you want to simulate again?\n[1]Yes or [0]No\n");
        scanf("%d",&opt);
    }while(opt!=0);
}
