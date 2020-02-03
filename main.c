#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <stdbool.h>
#include <unistd.h>
#include <time.h>


#define PORT 12345
#define SA struct sockaddr
void clientjoinchannel();
char buffer[1000000];
int server_socket, client_socket;
struct ussers{
    char token[21];
    char name[100];
    char channelname[100];
}clients[10000];
int registeredusers;
//char onlineusers[100][20];

int server_socket, client_socket;

void connecttoclient(){
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    struct sockaddr_in client;
    // Accept the data packet from client and verify
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");

    // Function for chatting between client and server
}
void Clientregister(){
    char checkstring[100];
    char checkstring2[100];
    char password[100];
    char username[100];
    sscanf(buffer,"%s %s %s",checkstring , checkstring2 , password);
    checkstring2[strlen(checkstring2) - 1] = 0;
    strcpy(username , checkstring2);
    strcat(checkstring2 , ".json");
    strcpy(checkstring , "database/clients/");
    strcat(checkstring , checkstring2);
    if(access(checkstring , F_OK) != -1){
        // file exists
        cJSON* error = cJSON_CreateObject();
        cJSON_AddStringToObject(error,"type" , "Error");
        cJSON_AddStringToObject(error,"content", "this username have been used");
        char* string;
        string = cJSON_Print(error);
        send(client_socket , string , strlen(string) , 0);
        return;
    } else {
        FILE* client = fopen(checkstring , "w");
        cJSON* Client = cJSON_CreateObject();
        cJSON_AddStringToObject(Client,"username" , username);
        cJSON_AddStringToObject(Client,"password", password);
        char* string;
        string = cJSON_Print(Client);
        fprintf(client , string);
        cJSON* success = cJSON_CreateObject();
        cJSON_AddStringToObject(success,"type" , "Successful");
        cJSON_AddStringToObject(success,"content", "");
        char* string2;
        string2 = cJSON_Print(success);
        send(client_socket , string2 , strlen(string2) , 0);
        printf("%s\n",string2);
        fclose(client);
        strcpy(clients[registeredusers].name , username);
        registeredusers++;
        return;
    }
}
void Clientlogin(){
    char checkstring[100];
    char checkstring2[100];
    char password[100];
    char username[100];
    sscanf(buffer,"%s %s %s",checkstring , checkstring2 , password);
    checkstring2[strlen(checkstring2) - 1] = 0;
    strcpy(username , checkstring2);
    strcat(checkstring2 , ".json");
    strcpy(checkstring , "database/clients/");
    strcat(checkstring , checkstring2);
    if(access(checkstring , F_OK) != -1) {
        FILE* client = fopen(checkstring , "r");
        char textjson[10000];
        int n = 0;
        while((textjson[n++] = getc(client)) != EOF);
        textjson[n] = 0;
        printf("%s\n",textjson);
        if(strcmp(password , cJSON_GetObjectItem(cJSON_Parse(textjson), "password")->valuestring) == 0){
            for(int i = 0 ; i < registeredusers; i++){
                if(strcmp(clients[i].name , username) == 0)
                     if(strcmp(clients[i].token , "") != 0){
                         cJSON* error = cJSON_CreateObject();
                         cJSON_AddStringToObject(error,"type" , "Error");
                         cJSON_AddStringToObject(error,"content", "this username is already online");
                         char* string;
                         string = cJSON_Print(error);
                         send(client_socket , string , strlen(string) , 0);
                         return;
                }
            }
            char random[62] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
            char authToken[21];
            srand(time(0));
            for(int i = 0; i < 20 ; i++){
                authToken[i] = random[rand() % 61];
            }
            printf("logintoken : %s\n",authToken);
            cJSON* AuthToken = cJSON_CreateObject();
            cJSON_AddStringToObject(AuthToken,"type" , "AuthToken");
            cJSON_AddStringToObject(AuthToken,"content", authToken);
            char* string;
            string = cJSON_Print(AuthToken);
            send(client_socket , string , strlen(string) , 0);
            strcpy(clients[registeredusers].token , authToken);
            strcpy(clients[registeredusers].name , username);
            printf("tokenstruct : %s\n",clients[registeredusers].token);
            printf("namestruct : %s\n",clients[registeredusers].name);
            registeredusers++;
            return;
        }
        else{
            cJSON* error = cJSON_CreateObject();
            cJSON_AddStringToObject(error,"type" , "Error");
            cJSON_AddStringToObject(error,"content", "password is incorrect");
            char* string;
            string = cJSON_Print(error);
            send(client_socket , string , strlen(string) , 0);
            return;
        }
    }
    else{
        cJSON* error = cJSON_CreateObject();
        cJSON_AddStringToObject(error,"type" , "Error");
        cJSON_AddStringToObject(error,"content", "this username is not available");
        char* string;
        string = cJSON_Print(error);
        send(client_socket , string , strlen(string) , 0);
        return;
    }
}
void clientCreateChannel(){
    char checkstring[100];
    char checkstring2[100];
    char checkstring3[100];
    char authtoken[100];
    char channelname[100];
    int structnum;
    sscanf(buffer,"%s %s %s %s",checkstring , checkstring2 , checkstring3 , authtoken);
    printf("creatchannel token : %s\n",authtoken);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(authtoken , clients[i].token) != 0) {
            printf("\t1\n");
            if (i == registeredusers - 1) {
                cJSON *error = cJSON_CreateObject();
                cJSON_AddStringToObject(error, "type", "Error");
                cJSON_AddStringToObject(error, "content", "this username is not online");
                char *string;
                string = cJSON_Print(error);
                send(client_socket, string, strlen(string), 0);
                return;
            }
        }
        else {
                structnum = i;
                printf("structnum : %d\n",structnum);
                break;
        }
    }
    checkstring3[strlen(checkstring3) - 1] = 0;
    strcpy(channelname , checkstring3);
    strcat(checkstring3 , ".json");
    strcpy(checkstring2 , "database/channels/");
    strcat(checkstring2 , checkstring3);
    if(access(checkstring2 , F_OK) != -1){
        cJSON* error = cJSON_CreateObject();
        cJSON_AddStringToObject(error,"type" , "Error");
        cJSON_AddStringToObject(error,"content", "this channel name has been used");
        char* string;
        string = cJSON_Print(error);
        send(client_socket , string , strlen(string) , 0);
        return;
    }
    else{
        strcpy(clients[structnum].channelname , checkstring2);
        FILE* channel = fopen(checkstring2 , "w");
        cJSON * Client = cJSON_CreateObject();
        cJSON * messages = cJSON_AddArrayToObject(Client , "messages");
        cJSON * members = cJSON_AddArrayToObject(Client , "members");
        cJSON * membernum = cJSON_AddArrayToObject(Client , "membernum");
        char* string;
        string = cJSON_Print(Client);
        fprintf(channel , string);
        printf("in file we must have: %s\n",string);
        cJSON* success = cJSON_CreateObject();
        cJSON_AddStringToObject(success,"type" , "Successful");
        cJSON_AddStringToObject(success,"content", "");
        char* string2;
        string2 = cJSON_Print(success);
        send(client_socket , string2 , strlen(string2) , 0);
        fclose(channel);
        clientjoinchannel();
        return;
    }
}

void clientjoinchannel(){
    char checkstring[100];
    char checkstring2[100];
    char checkstring3[100];
    char authtoken[100];
    char channelname[100];
    int membernum;
    sscanf(buffer,"%s %s %s %s",checkstring , checkstring2 , checkstring3 , authtoken);
   printf("clientjoinchannel token: %s\n",authtoken);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(authtoken , clients[i].token) != 0){
            if(i == registeredusers - 1) {
                cJSON *error = cJSON_CreateObject();
                cJSON_AddStringToObject(error, "type", "Error");
                cJSON_AddStringToObject(error, "content", "this username is not online");
                char *string;
                string = cJSON_Print(error);
                send(client_socket, string, strlen(string), 0);
                printf("eror: %s",string);
                return;
            }
        }
        else{
            membernum = i;
            break;
        }
    }
    checkstring3[strlen(checkstring3) - 1] = 0;
    strcpy(channelname , checkstring3);
    strcat(checkstring3 , ".json");
    strcpy(checkstring2 , "database/channels/");
    strcat(checkstring2 , checkstring3);
    if(access(checkstring2 , F_OK) != -1){
        printf("open file\n");
        FILE* channel = fopen(checkstring2, "r");
        char textjson[100000];
        int n = 0;
        while((textjson[n++] = getc(channel)) != EOF);
        textjson[n] = 0;
        printf("textjsonjoinchannel : %s\n",textjson);
        fclose(channel);
        char* string;
        cJSON * new = cJSON_Parse(textjson);
        cJSON * buffer1 = cJSON_GetObjectItemCaseSensitive(new , "members");
        cJSON * memnum = cJSON_GetObjectItemCaseSensitive(new , "membernum");
        cJSON * content = cJSON_GetObjectItem(new , "content");
        for(int  j = 0 ; j < cJSON_GetArraySize(content); j++){
            if(strcmp(clients[membernum].name , cJSON_GetArrayItem(content, j)->valuestring) == 0){
                cJSON* error = cJSON_CreateObject();
                cJSON_AddStringToObject(error,"type" , "Error");
                cJSON_AddStringToObject(error,"content", "this member is already a member of the channel!");
                char* string;
                string = cJSON_Print(error);
                send(client_socket , string , strlen(string) , 0);
                return;
            }
        }
        cJSON *  memnumber = cJSON_CreateNumber(0);
        cJSON_AddItemToArray(memnum , memnumber);
        cJSON * memname = cJSON_CreateString(clients[membernum].name);
        cJSON_AddItemToArray(buffer1 , memname);
        //cJSON * buffer2 = cJSON_CreateObject();
        //cJSON_AddStringToObject(buffer2 , "content" , clients[membernum].name);
        //cJSON_AddItemToArray(buffer1 , buffer2);
        char* string3;
        string3 = cJSON_Print(new);
        printf("adding to channel : %s\n",string3);
        channel = fopen(checkstring2, "w");
        fputs(string3 , channel);
        printf("%s\n",string3);
        fclose(channel);


        channel = fopen(checkstring2, "r");
        n = 0;
        while((textjson[n++] = getc(channel)) != EOF);
        textjson[n] = 0;
        fclose(channel);
        printf("now in file after adding to chanel : %s\n",textjson);


        strcpy(clients[membernum].channelname , checkstring2);
        printf("clients[membernum].channelname in join channel : %s",clients[membernum].channelname);
        cJSON* success = cJSON_CreateObject();
        cJSON_AddStringToObject(success,"type" , "Successful");
        cJSON_AddStringToObject(success,"content", "");
        char* string2;
        string2 = cJSON_Print(success);
        send(client_socket , string2 , strlen(string2) , 0);
        return;
    }
    else{
        cJSON* error = cJSON_CreateObject();
        cJSON_AddStringToObject(error,"type" , "Error");
        cJSON_AddStringToObject(error,"content", "this channel is not available");
        char* string;
        string = cJSON_Print(error);
        send(client_socket , string , strlen(string) , 0);
        return;
    }
}
void sendmessage(){
    char clientmessage[100000];
    char channelname[100];
    char clientname[100];
    strcpy(clientmessage , buffer + 5);
    printf("buffer : %s\n",buffer);
    printf("buffers clientmessage:%s\n",clientmessage);
    char token[22];
    int size = strlen(clientmessage) - 1;
    printf("size : %d\n",size);
    int j = 0;
    for(int i = 21 ; i >= 0 ; i--){
        if(i <= 19){
            token[j] = clientmessage[size - 1 - i];
            j++;
        }
        clientmessage[size - 1 - i] = 0;
    }
    //clientmessage[]
    printf("client message: %s\n",clientmessage);
    //printf("send token : %s111\n",token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strncmp(token , clients[i].token , 20) == 0){
            strcpy(channelname , clients[i].channelname);
            strcpy(clientname , clients[i].name);
            break;
        }
    }
    FILE* channel = fopen(channelname, "r");
    char textjson[100000];
    int n = 0;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    //printf("channel add : %s\n",channelname);
    //printf("sendtextjson : %s\n",textjson);
    channel = fopen(channelname , "w");
    char* string;
    cJSON * new = cJSON_Parse(textjson);
    cJSON * buffer1 =  cJSON_GetObjectItemCaseSensitive(new , "messages");
    cJSON * message = cJSON_CreateObject();
    cJSON_AddStringToObject(message , "sender" , clientname);
    cJSON_AddStringToObject(message , "content" , clientmessage);
    cJSON_AddItemToArray(buffer1 , message);
    string = cJSON_Print(new);
    printf("send string: %s\n",string);
    fprintf(channel , string);
    fclose(channel);
    cJSON* success = cJSON_CreateObject();
    cJSON_AddStringToObject(success,"type" , "Successful");
    cJSON_AddStringToObject(success,"content", "");
    char* string2;
    string2 = cJSON_Print(success);
    send(client_socket , string2 , strlen(string2) , 0);
}
void clientrefreh(){
    char firststring[10];
    char token[100];
    int memid;
    sscanf(buffer ,"%s %s",firststring , token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(clients[i].token , token) == 0){
            memid = i;
            if(strcmp(clients[memid].channelname , "") == 0){
                cJSON* error = cJSON_CreateObject();
                cJSON_AddStringToObject(error,"type" , "Error");
                cJSON_AddStringToObject(error,"content", "this member are not in any channel");
                char* string;
                string = cJSON_Print(error);
                send(client_socket , string , strlen(string) , 0);
                return;
            }
            break;
        }
    }
    FILE * channel = fopen(clients[memid].channelname , "r");
    char textjson[100000];
    int n = 0 , membernum , numinjson;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    ///printf("textjson refresh : %s",textjson);
    cJSON * new = cJSON_Parse(textjson);
    cJSON * membernumber = cJSON_GetObjectItem(new , "membernum");
    cJSON * membername = cJSON_GetObjectItem(new , "members");
    for(int i = 0 ; i < cJSON_GetArraySize(membername) ; i++){
        if(strcmp(cJSON_GetArrayItem(membername , i) -> valuestring , clients[memid].name) == 0){
            membernum = cJSON_GetArrayItem(membernumber , i) -> valueint;
            printf("membernum: %d\n",membernum);
            numinjson = i;
            printf("numinjson : %d\n",numinjson);
        }
    }
    cJSON * Client = cJSON_CreateObject();
    cJSON_AddStringToObject(Client , "type" , "List");
    cJSON * messages = cJSON_AddArrayToObject(Client , "content");
    cJSON * oldmessage = cJSON_GetObjectItem(new , "messages");
    for(int i = membernum ; i < cJSON_GetArraySize(oldmessage) ; i++){
        cJSON * mycontent = cJSON_GetArrayItem(oldmessage , i);
        cJSON * sender = cJSON_CreateObject();
        cJSON_AddItemToArray(messages , sender);
        printf("sender : %s\n" , cJSON_GetObjectItem(mycontent , "sender") -> valuestring);
        printf("message : %s\n" , cJSON_GetObjectItem(mycontent , "content") -> valuestring);
        cJSON_AddStringToObject(sender , "sender" , cJSON_GetObjectItem(mycontent , "sender") -> valuestring);
        cJSON_AddStringToObject(sender , "content" , cJSON_GetObjectItem(mycontent , "content") -> valuestring);
    }
    cJSON * newnum =  cJSON_CreateNumber(cJSON_GetArraySize(oldmessage));
    cJSON_ReplaceItemInArray(membernumber , numinjson , newnum);
    channel = fopen(clients[memid].channelname , "w");
    char* string2;
    string2 = cJSON_Print(new);
    fprintf(channel , string2);
    fclose(channel);
    char* string;
    string = cJSON_Print(Client);
    printf("last (send) string : %s\n",string);
    send(client_socket , string , strlen(string) , 0);
}
void channelmembers(){
    char first[100];
    char second[100];
    char token[50];
    char channelname[100];
    sscanf(buffer , "%s %s %s", first,second ,token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(clients[i].token , token) == 0){
            if(strcmp(clients[i].channelname , "") == 0){
                cJSON* error = cJSON_CreateObject();
                cJSON_AddStringToObject(error,"type" , "Error");
                cJSON_AddStringToObject(error,"content", "this member are not in any channel");
                char* string;
                string = cJSON_Print(error);
                send(client_socket , string , strlen(string) , 0);
                return;
            }
            else{
                strcpy(channelname , clients[i].channelname);
                break;
            }
        }
    }
    FILE * channel = fopen(channelname , "r");
    char textjson[1000000];
    int n = 0;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    cJSON * new = cJSON_Parse(textjson);
    cJSON * members = cJSON_GetObjectItem(new , "members");
    cJSON * Client = cJSON_CreateObject();
    cJSON_AddStringToObject(Client , "type" , "List");
    cJSON * newmembers = cJSON_AddArrayToObject(Client , "content");
    cJSON_AddItemToArray(newmembers , cJSON_GetArrayItem(members , 0));
    char* string;
    string = cJSON_Print(Client);
    send(client_socket , string , strlen(string) , 0);
}
void clientleave(){
    char first[100];
    char token[50];
    char channelname[100];
    char membername[100];
    sscanf(buffer , "%s %s" , first , token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(clients[i].token , token) == 0){
            if(strcmp(clients[i].channelname , "") == 0){
                cJSON* error = cJSON_CreateObject();
                cJSON_AddStringToObject(error,"type" , "Error");
                cJSON_AddStringToObject(error,"content", "this member are not in any channel to leave it");
                char* string;
                string = cJSON_Print(error);
                send(client_socket , string , strlen(string) , 0);
                return;
            } else{
                strcpy(channelname , clients[i].channelname);
                strcpy(membername , clients[i].name);
                break;
            }
        }
    }
    FILE * channel = fopen(channelname , "r");
    char textjson[1000000];
    int n = 0;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    fopen(channelname , "w");
    cJSON * new = cJSON_Parse(textjson);
    cJSON * newmembers = cJSON_GetObjectItemCaseSensitive(new , "members");
    cJSON * newnubers = cJSON_GetObjectItemCaseSensitive(new , "membernum");
    for(int i = 0 ; i < cJSON_GetArraySize(newmembers) ; i++){
        if(strcmp(membername , cJSON_GetArrayItem(newmembers , i) -> valuestring) == 0){
            cJSON_DeleteItemFromArray(newmembers , i);
            cJSON_DeleteItemFromArray(newnubers , i);
            break;
        }
    }
    char* string;
    string = cJSON_Print(new);
    fprintf(channel , string);
    fclose(channel);
    cJSON* success = cJSON_CreateObject();
    cJSON_AddStringToObject(success,"type" , "Successful");
    cJSON_AddStringToObject(success,"content", "");
    char* string2;
    string2 = cJSON_Print(success);
    send(client_socket , string2 , strlen(string2) , 0);
}
void clientlogout(){
    char first[100];
    char token[50];
    sscanf(buffer , "%s %s",first , token);
    printf("logout buffer : %s\n",buffer);
    printf("logout token : %s\n",token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(clients[i].token , token) == 0){
            strcpy(clients[i].token , "");
            strcpy(clients[i].name , "");
            strcpy(clients[i].channelname , "");
            cJSON* success = cJSON_CreateObject();
            cJSON_AddStringToObject(success,"type" , "Successful");
            cJSON_AddStringToObject(success,"content", "");
            char* string2;
            string2 = cJSON_Print(success);
            send(client_socket , string2 , strlen(string2) , 0);
            return;
        }
    }
}

void client_find_user(){
    char finduser[100];
    char username[100];
    char token[100];
    char channelname[200];
    sscanf(buffer , "%s %s %s",finduser , username , token);
    printf("find user token : %s\n",token);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strcmp(clients[i].token , token) == 0){
            if(strcmp(clients[i].channelname , "") == 0){
                cJSON* error = cJSON_CreateObject();
                cJSON_AddStringToObject(error,"type" , "Error");
                cJSON_AddStringToObject(error,"content", "this member are not in any channel");
                char* string;
                string = cJSON_Print(error);
                send(client_socket , string , strlen(string) , 0);
                return;
            }
            else{
                strcpy(channelname , clients[i].channelname);
                break;
            }
        }
    }
    printf("you are in find_users\n");
    printf("channel name : %s\n",channelname);
    FILE * channel = fopen(channelname , "r");
    char textjson[1000000];
    int n = 0;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    char string[500];
    cJSON * new = cJSON_Parse(textjson);
    cJSON * members = cJSON_GetObjectItem(new , "members");
    for(int i = 0 ; i < cJSON_GetArraySize(members) ; i++){
        if(strcmp(username , cJSON_GetArrayItem(members , i) -> valuestring) == 0){
            strcpy(string , username);
            strcat(string , " are in this channel\n");
            printf("%s\n",string);
            send(client_socket , string , sizeof(string) , 0);
            return;
        }
    }
    strcpy(string , username);
    strcat(string , " are not in this channel\n");
    printf("string2 : %s",string);
    send(client_socket , string , sizeof(string) , 0);
}
void client_find_message(){
    char clientsearchmessage[100000];
    char channelname[100];
    char clientname[100];
    strcpy(clientsearchmessage , buffer + 13);
    printf("buffer search(message) : %s\n",buffer);
    printf("buffers client search message:%s\n",clientsearchmessage);
    char token[22];
    int size = strlen(clientsearchmessage) - 1;
    printf("size : %d\n",size);
    int j = 0;
    for(int i = 21 ; i >= 0 ; i--){
        if(i <= 19){
            token[j] = clientsearchmessage[size - 1 - i];
            j++;
        }
        clientsearchmessage[size - 1 - i] = 0;
    }
    printf("client search message: %s\n",clientsearchmessage);
    for(int i = 0 ; i < registeredusers ; i++){
        if(strncmp(token , clients[i].token , 20) == 0){
            strcpy(channelname , clients[i].channelname);
            strcpy(clientname , clients[i].name);
            break;
        }
    }
    FILE* channel = fopen(channelname, "r");
    char textjson[100000];
    int n = 0;
    while((textjson[n++] = getc(channel)) != EOF);
    textjson[n] = 0;
    fclose(channel);
    cJSON * new = cJSON_Parse(textjson);
    cJSON * oldmessage = cJSON_GetObjectItem(new , "messages");
    cJSON * Client = cJSON_CreateObject();
    cJSON_AddStringToObject(Client , "type" , "List");
    cJSON * messages = cJSON_AddArrayToObject(Client , "content");
    for(int i = 0 ; i < cJSON_GetArraySize(oldmessage) ; i++){
        cJSON * mycontent = cJSON_GetArrayItem(oldmessage , i);
        printf("sender : %s\n" , cJSON_GetObjectItem(mycontent , "sender") -> valuestring);
        printf("message : %s\n" , cJSON_GetObjectItem(mycontent , "content") -> valuestring);
        for(int j = 0 ; j < strlen(cJSON_GetObjectItem(mycontent , "content") -> valuestring) ; j++){
            if(strncmp(clientsearchmessage , j + cJSON_GetObjectItem(mycontent , "content") -> valuestring , strlen(clientsearchmessage)) == 0) {
                cJSON *sender = cJSON_CreateObject();
                cJSON_AddItemToArray(messages, sender);
                cJSON_AddStringToObject(sender, "sender", cJSON_GetObjectItem(mycontent, "sender")->valuestring);
                cJSON_AddStringToObject(sender, "content", cJSON_GetObjectItem(mycontent, "content")->valuestring);
                break;
            }
        }
    }
    char* string;
    string = cJSON_Print(Client);
    printf("last (send) search string : %s\n",string);
    send(client_socket , string , strlen(string) , 0);
}
int main(){
    struct sockaddr_in server, client;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
    if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");
    // Now server is ready to listen and verify
    while(1) {
        connecttoclient();
        memset(buffer , 0 , strlen(buffer));
        recv(client_socket , buffer , sizeof(buffer) , 0);
        printf("main buffer : %s\n",buffer);
        ///TODO recive from client
        char checkstring[50];
        sscanf(buffer, "%s", checkstring);
        if (strcmp(checkstring, "register") == 0) {
            Clientregister();
            closesocket(client_socket);
        }
        else if (strcmp(checkstring, "login") == 0) {
            Clientlogin();
            closesocket(client_socket);
        }
        else if (strcmp(checkstring, "create") == 0) {
            clientCreateChannel();
            closesocket(client_socket);
        }
        else if (strcmp(checkstring, "join") == 0) {
            clientjoinchannel();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring, "send") == 0){
            sendmessage();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "refresh") == 0){
            clientrefreh();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "channel") == 0){
            channelmembers();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "leave") == 0){
            clientleave();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "logout") == 0){
            clientlogout();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "find_user") == 0){
            client_find_user();
            closesocket(client_socket);
        }
        else if(strcmp(checkstring , "find_message") == 0){
            client_find_message();
            closesocket(client_socket);
        }
        memset(buffer, 0, sizeof(buffer));
    }
}