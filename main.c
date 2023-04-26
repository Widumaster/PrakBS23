
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define BUFSIZE 1024 // Größe des Buffers
#define TRUE 1
#define ENDLOSSCHLEIFE 1
#define PORT 5678
#define NUM_OF_CHILDS 4
#define SEGSIZE sizeof(KeyValue)

int checkKey(int key);

typedef struct {
    int keyStore[1000];
    int valueStore[1000];
} KeyValue;

KeyValue kv;
int arrayCounter = 0;

void put(int key, int value) {
    if (arrayCounter < sizeof(kv.keyStore)) {
        kv.keyStore[key] = key;
        kv.valueStore[key] = value;
        arrayCounter++;
    }
}

int putFun(int cfd, int bytes_read, char *in, int *keyHolder, int *valueHolder) {
    write(cfd, "Key: ", strlen("Key: "));
    bytes_read = read(cfd, in, BUFSIZE);
    *keyHolder = atoi(in);

    //Get Value
    write(cfd, "Value: ", strlen("Value: "));
    bytes_read = read(cfd, in, BUFSIZE);
    *valueHolder = atoi(in);

    printf("Put Key %i with Value : %i \n ", *keyHolder, *valueHolder);

    put(*keyHolder, *valueHolder);
}

void get(int key) {
    for (int i = 0; i < sizeof(kv.keyStore); i++) {
        if (key == kv.keyStore) {
            printf("%i \n", kv.valueStore[key + 1]);
            break;
        }
    }
}

int getFun(int cfd, int bytes_read, char *in, int *keyHolder, int *valueHolder) {
    write(cfd, "Key: ", strlen("Key: "));
    bytes_read = read(cfd, in, BUFSIZE);
    int keyValue = atoi(in);
    int keyExists = checkKey(keyValue);
    if (keyExists) {
        get(keyValue);
        char valueStr[BUFSIZE];
        sprintf(valueStr, "%d", kv.valueStore[keyValue]);
        printf(valueStr);
        write(cfd, "Value: ", strlen("Value: "));
        write(cfd, valueStr, strlen(valueStr));
        write(cfd, "\n", strlen("\n"));
    } else {
        write(cfd, "Schluessel nicht vorhanden", strlen("Schluessel nicht vorhanden"));
        write(cfd, "\n", strlen("\n"));
    }
}

int checkKey(int key) {
    for (int i = 0; i < sizeof(kv.keyStore); i++) {
        if (key == kv.keyStore[i]) {
            return 1;
        }
    }
    return 0;
}

void del(int key) {
    for (int i = 0; i < sizeof(kv.keyStore); i++) {
        if (key == kv.keyStore[i]) {
            kv.keyStore[i] = 0;
            kv.valueStore[i] = 0;
            arrayCounter--;
            printf("Deleted Key %d with Value : %d\n", key, kv.valueStore[i]);
            break;
        }
    }
}

int delFun(int cfd, int bytes_read, char *in, int *keyHolder, int *valueHolder) {
    write(cfd, "Key: ", strlen("Key: "));
    bytes_read = read(cfd, in, BUFSIZE);
    int keyValue = atoi(in);
    int keyExists = checkKey(keyValue);
    if (keyExists) {
        del(keyValue);
        write(cfd, "Schluessel geloescht\n", strlen("Schluessel geloescht\n"));
    } else {
        write(cfd, "Schluessel nicht vorhanden\n", strlen("Schluessel nicht vorhanden\n"));
    }
}

int main() {
    int rfd; // Rendevouz-Descriptor/ File-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFSIZE]; // Daten vom Client an den Server
    int bytes_read = 1; // Anzahl der Bytes, die der Client geschickt hat

    int i, id, *shar_mem;   /*  id für das Shared Memory Segment        */
    /*  mit *shar_mem kann der im Shared Memory */
    /*  gespeicherte Wert verändert werden      */
    int pid[NUM_OF_CHILDS]; /*  enthält die PIDs der Kindprozesse       */

    /* Nun wird das Shared Memory Segment angefordert, an den Prozess   */
    /* angehängt, und der dort gespreicherte Wert auf 0 gesetzt         */
    id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    shar_mem = (int *) shmat(id, 0, 0);
    *shar_mem = 0;


    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0) {
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET; //IPv4
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT); //Port
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server)); // Socket an eine Adresse binden
    if (brt < 0) {
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0) {
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    int keyHolder;
    int valueHolder;

    while (ENDLOSSCHLEIFE) {
        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        int pidD = fork();
        if (pidD != 0) {
            printf("After Forking \n");
            // Lesen von Daten, die der Client schickt
            bytes_read = read(cfd, in, BUFSIZE);
            printf("After Client Input \n");

            // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
            while (bytes_read > 0) {
                printf("sending back the %d bytes I received...\n", bytes_read);
                printf("GETPid: %i \n", getpid());
                bytes_read = read(cfd, in, BUFSIZE);
                //PUT
                if (strncmp(in, "put", 3) == 0) {
                    //putFun(cfd, bytes_read, in, &keyHolder, &valueHolder);
                    KeyValue *shar_mem = NULL;
                    write(cfd, "Key: ", strlen("Key: "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    keyHolder = atoi(in);

                    //Get Value
                    write(cfd, "Value: ", strlen("Value: "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    valueHolder = atoi(in);
                    printf("Put Key %i with Value : %i \n ", keyHolder, valueHolder);

                    put(keyHolder, valueHolder);
                    //GET
                } else if (strncmp(in, "get", 3) == 0) {
                    //getFun(cfd, bytes_read, in, &keyHolder, &valueHolder);
                    write(cfd, "Key: ", strlen("Key: "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    int keyValue = atoi(in);
                    int keyExists = checkKey(keyValue);
                    if (keyExists) {
                        get(keyValue);
                        char valueStr[BUFSIZE];
                        sprintf(valueStr, "%d", kv.valueStore[keyValue]);
                        printf(valueStr);
                        write(cfd, "Value: ", strlen("Value: "));
                        write(cfd, valueStr, strlen(valueStr));
                        write(cfd, "\n", strlen("\n"));
                    } else {
                        write(cfd, "Schluessel nicht vorhanden", strlen("Schluessel nicht vorhanden"));
                        write(cfd, "\n", strlen("\n"));
                    }
                }
                    //DEL
                else if (strncmp(in, "del", 3) == 0) {
                    //delFun(cfd, bytes_read, in, &keyHolder, &valueHolder);
                    write(cfd, "Key: ", strlen("Key: "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    int keyValue = atoi(in);
                    int keyExists = checkKey(keyValue);
                    if (keyExists) {
                        del(keyValue);
                        write(cfd, "Schluessel geloescht\n", strlen("Schluessel geloescht\n"));
                    } else {
                        write(cfd, "Schluessel nicht vorhanden\n", strlen("Schluessel nicht vorhanden\n"));
                    }
                    //QUIT
                } else if (strncmp(in, "quit", 4) == 0) {
                    printf("Verbindung abgebrochen\n");
                    shmdt(shar_mem);
                    shmctl(id, IPC_RMID, 0);
                    close(cfd);
                } else {
                    printf("Fehlgeschlagen \n");
                }

            }
        }
        close(cfd);
    }

    // Rendevouz Descriptor schließen
    close(rfd);
}
