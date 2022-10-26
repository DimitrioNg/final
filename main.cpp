//Веб-сервер должен запускаться командой: /home/box/final/final -h <ip> -p <port> -d <directory>
//
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <vector>

#include <unistd.h> //getopt
#include <syslog.h> //syslog
#include <sys/wait.h> //waitpid
#include <sys/types.h> //bind
#include <sys/socket.h> //bind, listen
#include <netinet/in.h>
#include <arpa/inet.h> //inet_pton
#include <sys/epoll.h> //epoll

#include "lib/worker.h"
#include "lib/fdtransceiver.h"
#include "lib/unblock.h"
#include "lib/daemonizator.h"

#define MAX_WORKERS 5
#define MAX_EPOLL_EVENTS 20
#define MAX_THREADS 5

typedef int worker_id_t;
typedef int worker_fd_t;



int log_to_stderr = 0;

struct cmd_set{
std::string port = "";
std::string ip = "";
std::string working_dir = "";
};

struct worker_struct {
    int fd; // передаваемый воркеру файловый дескриптор
    pthread_t tid;
    int epoll_fd; // передаваемый воркеру дескриптор epoll
    bool err; // 0 - no errorrs; -1 - error is present
    bool run; // 1 - worker is running
    std::string working_dir; //передаваемая воркеру, рабочая директория
    pthread_cond_t start;   //условная переменная запускающая воркера
    pthread_mutex_t start_mutex; 
    };

//структура для очереди  передачи сокетов от главного процесса (принимающего входящие соединения) к синхронизатору, обслуживающему воркетров
struct fd_fifo_struct{
    std::queue<int> fd_queue;
    pthread_mutex_t fd_queue_mutex;
    };

typedef std::map<worker_id_t, worker_struct> workers_map;

//структура очереди ожидающих воркеров, буфер + мьютекс
struct waiting_workers_fifo_struct{
    std::queue<std::pair<worker_id_t, worker_fd_t>> fifo_buf;
    pthread_cond_t work_done; //условная переменная сигнализирующая о том что воркер окончил свою работу
    pthread_mutex_t mutex;
    };

typedef std::map<worker_id_t, worker_struct> workers_map;

struct  workers_struct{
    pthread_mutex_t workers_mutex; //мьютекс доступа к структуре воркеров
    workers_map workers_map;
    };

//структура передачи аргументов потоку синхронизатора
struct syncro_args_struct{
    fd_fifo_struct &fd_fifo;
    workers_struct &workers;
    waiting_workers_fifo_struct &waiting_workers;
    pthread_cond_t alarm;
    pthread_mutex_t alarm_mutex;
    };

//структура передачи аргументов потоку воркера
struct worker_args_struct{
    workers_struct &workers;
    waiting_workers_fifo_struct &waiting_workers;
    int worker_id;
    };

void get_cmdstr_param (int &argc, char *argv[], std::string params, cmd_set &cmd_set){
int cp_res = 0;
    while ( (cp_res = getopt(argc, argv, "h:p:d:")) != -1){
		switch (cp_res){
		    case 'h': 
                //std::cout << "found argument h = " << optarg << std::endl;
                cmd_set.ip = optarg;
            break;
		    case 'p': 
                //std::cout << "found argument p = " << optarg << std::endl;
                cmd_set.port = optarg;
            break;
		    case 'd':
                //std::cout << "found argument d = " << optarg << std::endl;
                cmd_set.working_dir = optarg;  
            break;
		    }
	    }
    }

void workers_creator(workers_struct &workers, waiting_workers_fifo_struct &waiting_workers, int epoll_fd, std::string working_dir){
    pthread_t worker_tid;
    ptread_mutex_init(&workers.workers_mutex, NULL);

    worker_struct worker;
    worker.run = false;
    worker.err = false;
    worker.epoll_fd = epoll_fd;
    worker.fd = 0;

    //заполняем воркеров ожидающих работу
    if(pthread_mutex_init(&waiting_workers.mutex, NULL) != 0){
            std::cout << "Mutex Creation ERROR!" << std::endl;
		    //return 0;
            }
    if(pthread_cond_init(&waiting_workers.work_done, NULL) !=0){
            std::cout << "Condition Variable Creation ERROR!" << std::endl;
            //return 0;
            }    
    std::pair<worker_id_t, worker_fd_t> pair = {0, 0};
    for (int i =0; i <= MAX_WORKERS; i++){
        //дозаполняем структуру воркера
        if(pthread_mutex_init(&worker.start_mutex, NULL) != 0){
            std::cout << "Condition Variable Creation ERROR!" << std::endl;
		    //return 0;
            }
        if(pthread_cond_init(&worker.start, NULL) !=0){
            std::cout << "Condition Variable Creation ERROR!" << std::endl;
            //return 0;
            }
        workers.workers_map[i] = worker; //добовляем очередного воркера   
        pair.first = i;
        waiting_workers.fifo_buf.push(pair); //заполняем очередь свободных воркеров
        }
         

    }


int main(int argc, char **argv){
    workers_map workers_map;
    fd_fifo_struct fd_fifo;
    waiting_workers_fifo_struct waiting_workers_fifo;
    worker_struct worker;
    workers_struct workers;
    
    
    
	cmd_set cmd_set; // структура с параметрами коммандной строки
	char *cmd;
    get_cmdstr_param (argc, argv, "h:p:d:", cmd_set);
    //std::string output_str = "Start Server! IP = " + cmd_set.ip + "; Port = " + cmd_set.port + "; Working Dir = " + cmd_set.working_dir;
    worker_str.working_dir = cmd_set.working_dir;

    if((cmd = strchr(argv[0], '/')) == NULL) cmd = argv[0];
    else cmd++; 
	//std::cout << "cmd: " << cmd << std::endl;
    daemonize(cmd);
    //daemonize2();
    //с этих пор процесс находится в состоянии демона, вывод сообщений возможет только через syslog 
    //std::cout << output_str << std::endl;
    //syslog(LOG_INFO, &output_str[0]);
    
    //создаем поток-синхронизатор



    //создадим мастерсокет
    int master_socket = socket( //- системный вызов socket
		AF_INET,		//параметр domain = протокол IPv4
		SOCK_STREAM,	//параметр type = протокол TCP
		IPPROTO_TCP		//параметр protocol = протокол TCP, возможно указать 0, что будет означать протокол по умолчанию
		); 
	if (master_socket == -1){ //проверяет открытие мастерсокета 
		syslog(LOG_ERR, "Unable to Create Master Socket %s", strerror(errno));
        exit(1);	
		}

    //Привязываем сокет к IP адресу и порту.
    //создадим структуру сокадр
	struct sockaddr_in master_socket_addr;
	master_socket_addr.sin_family = AF_INET; 
	inet_pton(AF_INET, &cmd_set.ip[0], &master_socket_addr.sin_addr);
	master_socket_addr.sin_port = htons(atoi(&cmd_set.port[0])); 

    // забиндим сокет
	if(bind(master_socket, (struct sockaddr *)(& master_socket_addr), sizeof(master_socket_addr)) == -1){
        syslog(LOG_ERR, "Unable to Bind Master Socket %s", strerror(errno));
        }
    
    //делаем мастерсокет неблокирующим
	set_nonblock(master_socket);

    // слушаем мастерсокет (подготавливаем сервер к входящим запросам клиентов)
    if(listen(master_socket, SOMAXCONN) == -1){
        syslog(LOG_ERR, "Unable to Start Listen the Master Socket %s", strerror(errno));
        exit(1);
        }

    // создаем дескриптор epoll для родительского процесса
	int master_epoll_fd =  epoll_create1(0);
	if (master_epoll_fd == -1){
		syslog(LOG_ERR, "Unable to Create Epoll FD %s", strerror(errno));
		exit(1);
		}

	// зарегистрируем наш дескриптор epoll_fd в epoll для этого создадим стректуру epoll_event
	struct epoll_event epoll_event_str;
	epoll_event_str.data.fd = master_socket;
	epoll_event_str.events = EPOLLIN; //укажем какие события будем отслеживать, в нашем случае доступность на чтение

	// регистрируем
	if (epoll_ctl(master_epoll_fd, EPOLL_CTL_ADD, master_socket, &epoll_event_str) == -1){
        syslog(LOG_ERR, "Unable to Registrate Epoll Master Socket Event %s", strerror(errno));
        exit(1);
		}
    
    while (1){
        // Создать список для хранения возвращенных событий, возвращенных ждать 
        struct epoll_event events_from_epoll[MAX_EPOLL_EVENTS] = {0};
        //std::cout << "Wait Event from Mastersocket!" << std::endl;
        // опросим список входящих событий
        int N = epoll_wait	(
							master_epoll_fd, //дескриптор
							events_from_epoll, //список событий
							MAX_EPOLL_EVENTS, // максимальное количество возвращаямых событий
							-1 //таймаут (-1 ждать вечно)
							);
        for(unsigned int i = 0; i < N; i++){
			if(events_from_epoll[i].data.fd == master_socket){ //событие от мастерсокета
				//std::cout << "Event from Mastersocket!" << std::endl;
                int slave_socket = accept(master_socket, 0, 0); //выполняем accept
				if (slave_socket == -1) {
					//std::cout << "Can not accept Slave Socket!!!" << std::endl;
                    syslog(LOG_ERR, "Unable to Accept Slave Socket %s", strerror(errno));
					}
				//std::cout << "Accept Slave Socket!!! FD = " << slave_socket << ";" << std::endl;
                epoll_event_str.data.fd = slave_socket;
                epoll_event_str.events = EPOLLONESHOT; //epoll будет сигнализировать о событии 1 раз!
                // регистрируем сокет дочернего процесса
	            if (epoll_ctl(master_epoll_fd, EPOLL_CTL_ADD, slave_socket, &epoll_event_str) == -1){
                    syslog(LOG_ERR, "Unable to Registrate Epoll Master Socket Event %s", strerror(errno));
                    exit(1);
		            }
				} 
            else{ // данные пришли от какого-то воркера, открываем поток для их обработки
                while(maxworkers <= MAXWORKER){ // по возможности создаем поток
                    pthread_t worker_tid;
                        //заполняем структуру воркера;
                        worker_struct.worker_fd = events_from_epoll[i].data.fd;

                        workers_map[worker_tid] = 
                    if (pthread_create(&worker_tid, NULL, worker_tread, &worker_str) != 0){
		                syslog(LOG_ERR, "Master: FD Transceiver Thread Creation ERROR! %s", strerror(errno));
                        / /std::cout << "Worker PID: " << getpid() << " FD Transceiver Thread Creation ERROR!" << std::endl;
		                }
                    }
                    else{
                                                 
                        }    
                //std::cout << "Master PID: " << getpid() << "; Ivent From Worker!" << std::endl;
                    syslog(LOG_ERR, "Master: read_fd return NO Master Socket! %s", strerror(errno));
                }    				
			} // end for!
    } //  end while!
    

	return 0;
    }
