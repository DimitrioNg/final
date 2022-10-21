#include <map>
#include <deque>

struct worker_args_struct{
    int socket_fd;
    std::string working_dir;
    };


int worker_rx_tx(int, std::string);

void* worker_tread(void*);

