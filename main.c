#include <unistd.h>
#include "socket.h"
#include "keyValueStore.h"


int main() {

    Server server = GetSocket();


    while (1) {

        pollManager(&server);

    }
    // Rendevouz Descriptor schließen
    close(server.ServerID);
}