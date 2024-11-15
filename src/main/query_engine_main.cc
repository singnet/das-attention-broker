#include <iostream>
#include <string>

#include <signal.h>

using namespace std;

void ctrl_c_handler(int) {
    std::cout << "Stopping QueryEngineServer..." << std::endl;
    //service.graceful_shutdown(); XXXXX
    std::cout << "Done." << std::endl;
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " PORT" << endl;
        exit(1);
    }
    unsigned int port = stoi(argv[1]);
    signal(SIGINT, &ctrl_c_handler);
    return 0;
}
