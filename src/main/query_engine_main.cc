#include <iostream>
#include <string>

#include <signal.h>

#include "AtomDBSingleton.h"
#include "Utils.h"
#include "DASNode.h"

using namespace std;

void ctrl_c_handler(int) {
    std::cout << "Stopping query engine server..." << std::endl;
    std::cout << "Done." << std::endl;
    exit(0);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " PORT" << endl;
        exit(1);
    }

    AtomDBSingleton::init();
    string server_id = "localhost:" + string(argv[1]);
    signal(SIGINT, &ctrl_c_handler);
    DASNode server(server_id);
    do {
        Utils::sleep(1000);
    } while (true);
    return 0;
}
