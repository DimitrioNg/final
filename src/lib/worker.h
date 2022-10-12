#include <map>

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

/*
int create_worker( int i,
                    int listen_fd, //прослушиваемый сокет
                    Workers &workers // ссылка на объект с описанием воркеров
                    );
*/
int create_worker( int,
                    int, //прослушиваемый сокет
                    Workers &, // ссылка на объект с описанием воркеров
                    std::string
                    );