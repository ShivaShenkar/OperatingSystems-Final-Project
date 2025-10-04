#include "server.hpp"

int main() {
    Server server(9034);  // Port number
    server.run();
    return 0;
}
