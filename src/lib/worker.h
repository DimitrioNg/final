#include <map>
#include <deque>

struct worker_fd_fifo_struct{
	pthread_mutex_t fd_queue_lock;
	pthread_cond_t fd_queue_cond;
    std::string working_dir;
	std::deque<std::pair<int, std::string>> fd_queue;
	};

class Workers{

public:
    void add (int, int, std::string);
    void print (void);
    int inc (void);
    void dec (int fd);
    int del_by_fd (int fd);
    int del_by_pid (pid_t);
     
private:
    struct worker{
    pid_t PID;
    unsigned int descriptor_counter;
    std::string worker_dir;
    };
    std::map<int,worker> workers_map;
    int last_fd;
    bool first_att = true;
};

void worker_rx( int, //файловый дескриптор сокета
                int, //epoll дескриптор
                worker_fd_fifo_struct &fd_fifo );

void* worker_tx_thread(void*);

int create_worker( int,
                    int, //прослушиваемый сокет
                    Workers &, // ссылка на объект с описанием воркеров
                    std::string
                    );