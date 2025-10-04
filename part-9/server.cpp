#include "server.hpp"
#include "algos.hpp"
#include "randomGraph.hpp"

#include <memory>
#include <thread>
#include <sstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>
#include <random>

using namespace std;

// ----------------- Thread-safe queue -----------------
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    mutable std::mutex mtx;
    std::condition_variable cv;
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            q.push(std::move(item));
        }
        cv.notify_one();
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty()) return false;
        item = std::move(q.front());
        q.pop();
        return true;
    }

    void wait_and_pop(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !q.empty(); });
        item = std::move(q.front());
        q.pop();
    }

    // convenience that returns an item by value
    T pop() {
        T item;
        wait_and_pop(item);
        return item;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }
};

// ----------------- Task definition -----------------
struct Task {
    int client_fd;
    int vertices;
    int edges;
    Graph g;                       // will be set in buildGraphStage
    std::string MSTResult;
    std::string FlowResult;
    std::string CliqueResult;
    std::string SCCResult;
    Task()
        : client_fd(-1), vertices(1), edges(0), g(1) {}
     Task(int fd, int v, int e)
        : client_fd(fd), vertices(v), edges(e), g(v) {}
};

// Queues used by pipeline stages
static ThreadSafeQueue<Task> incomingQ;   // tasks created by handle_client_message
static ThreadSafeQueue<Task> buildQ;      // tasks waiting to be graph-built
static ThreadSafeQueue<Task> mstQ;
static ThreadSafeQueue<Task> flowQ;
static ThreadSafeQueue<Task> cliqueQ;
static ThreadSafeQueue<Task> sccQ;
static ThreadSafeQueue<Task> mergeQ;

// merge bookkeeping
static std::mutex mergeMtx;
static std::unordered_map<int, Task> partialResults;

// ----------------- Server ctor/dtor/setup -----------------

Server::Server(int port) {
    sockfd = setup_socket(port);
    fds.push_back({sockfd, POLLIN, 0});
}

Server::~Server() {
    close(sockfd);
}

int Server::setup_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        cerr << "Error binding socket" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(fd, SOMAXCONN) < 0) {
        cerr << "Error listening on socket" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Server listening on port " << port << endl;
    return fd;
}

// ----------------- Pipeline stage functions -----------------

// buildGraphStage: generate Graph and fan out to four algorithm queues
// ----------------- Pipeline stage functions -----------------

// buildGraphStage: generate Graph and fan out to four algorithm queues
static void buildGraphStage() {
    try {
        while (true) {
            Task task = buildQ.pop();
            try {
                task.g = generateRandomGraph(task.vertices, task.edges);
            } catch (const std::exception& ex) {
                task.MSTResult = string("Graph generation error: ") + ex.what();
            } catch (const char* msg) {
                task.MSTResult = string("Graph generation error: ") + msg;
            } catch (...) {
                task.MSTResult = "Unknown error during graph generation";
            }

            mstQ.push(task);
            flowQ.push(task);
            cliqueQ.push(task);
            sccQ.push(task);
        }
    } catch (...) {
        cerr << "[buildGraphStage] Thread terminating due to uncaught exception" << endl;
    }
}


static void mstStage() {
    while (true) {
        Task task = mstQ.pop();
        try {
            std::unique_ptr<AlgoStrategy> algo(AlgoFactory::createAlgo("mst", task.g));
            task.MSTResult = algo->execute();
        }
        catch (const std::exception& ex) {
            task.MSTResult = string("MST error: ") + ex.what();
        }
        catch (const char* msg) {
            task.MSTResult = string("MST error: ") + msg;
        }
        catch (...) {
            task.MSTResult = "Unknown MST error";
        }
        mergeQ.push(std::move(task));
    }
}

static void flowStage() {
    while (true) {
        Task task = flowQ.pop();
        try {
            std::unique_ptr<AlgoStrategy> algo(AlgoFactory::createAlgo("flow", task.g));
            task.FlowResult = algo->execute();
        }
        catch (const std::exception& ex) {
            task.FlowResult = string("Flow error: ") + ex.what();
        }
        catch (const char* msg) {
            task.FlowResult = string("Flow error: ") + msg;
        }
        catch (...) {
            task.FlowResult = "Unknown Flow error";
        }
        mergeQ.push(std::move(task));
    }
}

static void cliqueStage() {
    while (true) {
        Task task = cliqueQ.pop();
        try {
            std::unique_ptr<AlgoStrategy> algo(AlgoFactory::createAlgo("clique", task.g));
            task.CliqueResult = algo->execute();
        }
        catch (const std::exception& ex) {
            task.CliqueResult = string("Clique error: ") + ex.what();
        }
        catch (const char* msg) {
            task.CliqueResult = string("Clique error: ") + msg;
        }
        catch (...) {
            task.CliqueResult = "Unknown Clique error";
        }
        mergeQ.push(std::move(task));
    }
}

static void sccStage() {
    while (true) {
        Task task = sccQ.pop();
        try {
            std::unique_ptr<AlgoStrategy> algo(AlgoFactory::createAlgo("scc", task.g));
            task.SCCResult = algo->execute();
        }
        catch (const std::exception& ex) {
            task.SCCResult = string("SCC error: ") + ex.what();
        }
        catch (const char* msg) {
            task.SCCResult = string("SCC error: ") + msg;
        }
        catch (...) {
            task.SCCResult = "Unknown SCC error";
        }
        mergeQ.push(std::move(task));
    }
}

// mergeStage: combine partial results for same client_fd and respond when all four present
static void mergeStage() {
    while (true) {
        Task part = mergeQ.pop();
        {
            std::lock_guard<std::mutex> lock(mergeMtx);
            Task &entry = partialResults[part.client_fd];

            // If entry is newly created, ensure we set client_fd and vertices/edges if needed
            if (entry.client_fd == -1) {
                entry.client_fd = part.client_fd;
                entry.vertices = part.vertices;
                entry.edges = part.edges;
            }

            if (!part.MSTResult.empty()) entry.MSTResult = part.MSTResult;
            if (!part.FlowResult.empty()) entry.FlowResult = part.FlowResult;
            if (!part.CliqueResult.empty()) entry.CliqueResult = part.CliqueResult;
            if (!part.SCCResult.empty()) entry.SCCResult = part.SCCResult;

            // check if we have all four results
            if (!entry.MSTResult.empty() &&
                !entry.FlowResult.empty() &&
                !entry.CliqueResult.empty() &&
                !entry.SCCResult.empty()) {

                std::string response = "MST:\n" + entry.MSTResult +
                                       "\n\nMax Flow:\n" + entry.FlowResult +
                                       "\n\nMax Clique:\n" + entry.CliqueResult +
                                       "\n\nSCC:\n" + entry.SCCResult;

                // send response and cleanup
                ssize_t sent = send(entry.client_fd, response.c_str(), response.size(), 0);
                (void)sent; // ignoring partial-send handling for brevity

                close(entry.client_fd);
                partialResults.erase(part.client_fd);
            }
        }
    }
}

// ----------------- Server main loop / handlers -----------------

void Server::run() {
    // Start pipeline threads once at the beginning
    std::thread(buildGraphStage).detach();
    std::thread(mstStage).detach();
    std::thread(flowStage).detach();
    std::thread(cliqueStage).detach();
    std::thread(sccStage).detach();
    std::thread(mergeStage).detach();

    cout << "Pipeline threads started." << endl;
    // Main polling loop
    while (true) {
        int ready = poll(&fds[0], fds.size(), -1);
        if (ready < 0) { cerr << "Error in poll\n"; exit(EXIT_FAILURE); }

        if (fds[0].revents & POLLIN) handle_new_connection();

        for (int i = 1; i < (int)fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                handle_client_message(i);
                i--; // adjust because we may erase
            }
        }
    }
}


void Server::handle_new_connection() {
    int client_fd = accept(sockfd, nullptr, nullptr);
    if (client_fd >= 0) {
        cout << "New client connected: fd=" << client_fd << endl;
        fds.push_back({client_fd, POLLIN, 0});
    }
}


void Server::handle_client_message(int index) {
    char buffer[4096] = {0};
    int n = read(fds[index].fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        close(fds[index].fd);
        fds.erase(fds.begin() + index);
        std::cout << "Client disconnected." << std::endl;
        return;
    }

    buffer[n] = '\0';
    std::stringstream ss(buffer);
    int vertices = 0, edges = 0;
    ss >> vertices >> edges;

    if (vertices <= 0 || edges < 0 || edges > vertices*(vertices-1)/2) {
        std::string err = "Invalid graph parameters.\n";
        send(fds[index].fd, err.c_str(), err.size(), 0);
        close(fds[index].fd);
        fds.erase(fds.begin() + index);
        return;
    }

    // Create a Task for the pipeline
    Task task(fds[index].fd, vertices, edges);

    // push to build queue (graph will be generated there)
    buildQ.push(std::move(task));
    // remove fd from poll list; pipeline will close after sending results
    fds.erase(fds.begin() + index);
}



