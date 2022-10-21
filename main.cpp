//Веб-сервер должен запускаться командой: /home/box/final/final -h <ip> -p <port> -d <directory>
//
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

#include <unistd.h> //getopt
#include <syslog.h> //syslog
#include <sys/wait.h> //waitpid
#include <sys/types.h> //bind
#include <sys/socket.h> //bind, listen
#include <netinet/in.h>
#include <arpa/inet.h> //inet_pton
#include <sys/epoll.h> //epoll

#include "src/lib/worker.h"
#include "src/lib/fdtransceiver.h"
#include "src/lib/unblock.h"
#include "src/lib/daemonizator.h"

#define MAX_WORKERS 5
#define MAX_EPOLL_EVENTS 20
#define MAX_THREADS 5

int log_to_stderr = 0;

struct cmd_set{
std::string port = "";
std::string ip = "";
std::string working_dir = "";
};

// определяем обработчик сигнала SIGCHLD посылаемого от воркеров
// в этом обработчике просто вычитывается код завершения процесса, для предотвращения накопления процессов зомби,
// а также, PID воркера сохраняется в векторе dwp_vect (dead worker PID) 
std::vector<pid_t> dwp_vect;
void handle_sigchld(int sig) {
    int saved_errno = errno;
    bool status = true;
    pid_t pid; 
    while(status){
        pid = waitpid((pid_t)(-1), 0, WNOHANG);
        if (pid > 0){
            dwp_vect.push_back(pid);
            }
        else status = false;    
        }
        errno = saved_errno;
    }

//void send_fd_to_worker(int sock, int fd);

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

int main(int argc, char **argv){
	cmd_set cmd_set; // структура с параметрами коммандной строки
    worker_args_struct worker_str;
	char *cmd;
    get_cmdstr_param (argc, argv, "h:p:d:", cmd_set);
    //std::string output_str = "Start Server! IP = " + cmd_set.ip + "; Port = " + cmd_set.port + "; Working Dir = " + cmd_set.working_dir;
    worker_str.working_dir = cmd_set.working_dir;

    if((cmd = strchr(argv[0], '/')) == NULL) cmd = argv[0];
    else cmd++; 
	//std::cout << "cmd: " << cmd << std::endl;
    //daemonize(cmd);
    daemonize2();
    //с этих пор процесс находится в состоянии демона, вывод сообщений возможет только через syslog 
    //std::cout << output_str << std::endl;
    //syslog(LOG_INFO, &output_str[0]);
    
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
	struct epoll_event master_socket_event;
	master_socket_event.data.fd = master_socket;
	master_socket_event.events = EPOLLIN; //укажем какие события будем отслеживать, в нашем случае доступность на чтение

	// регистрируем
	if (epoll_ctl(master_epoll_fd, EPOLL_CTL_ADD, master_socket, &master_socket_event) == -1){
        syslog(LOG_ERR, "Unable to Registrate Epoll Master Socket Event %s", strerror(errno));
        exit(1);
		}

    
    while (1){
        // Создать список для хранения возвращенных событий, возвращенных ждать 
        struct epoll_event master_epoll_events[MAX_EPOLL_EVENTS] = {0};
        //std::cout << "Wait Event from Mastersocket!" << std::endl;
        // опросим список входящих событий
        int N = epoll_wait	(
							master_epoll_fd, //дескриптор
							master_epoll_events, //список событий
							MAX_EPOLL_EVENTS, // максимальное количество возвращаямых событий
							-1 //таймаут (-1 ждать вечно)
							);
        for(unsigned int i = 0; i < N; i++){
			if(master_epoll_events[i].data.fd == master_socket){ //событие от мастерсокета
				//std::cout << "Event from Mastersocket!" << std::endl;
                int slave_socket = accept(master_socket, 0, 0); //выполняем accept
				if (slave_socket == -1) {
					//std::cout << "Can not accept Slave Socket!!!" << std::endl;
                    syslog(LOG_ERR, "Unable to Accept Slave Socket %s", strerror(errno));
					}
				//std::cout << "Accept Slave Socket!!! FD = " << slave_socket << ";" << std::endl;
                worker_str.socket_fd = slave_socket;
                pthread_t worker_tid;
                //std::cout << "Create Worker with FD: " << worker_str.socket_fd << ", Dir: " << worker_str.working_dir << std::endl;
                if (pthread_create(&worker_tid, NULL, worker_tread, &worker_str) != 0){
		            syslog(LOG_ERR, "Master: FD Transceiver Thread Creation ERROR! %s", strerror(errno));
                    //std::cout << "Worker PID: " << getpid() << " FD Transceiver Thread Creation ERROR!" << std::endl;
		            }
                //else std::cout << "FD Transceiver Thread TID: " << worker_tid << " has Created" << std::endl;
                //отсоединяем поток
                if (pthread_detach(worker_tid) != 0) {
                    syslog(LOG_ERR, "Master: Can Not Detach Tread! %s", strerror(errno));
                    //std::cout <<  "Can Not Detach Tread TID: " << worker_tid << std::endl << std::flush;
                    //exit(4);
                    }
                //else std::cout << "FD Transceiver Thread TID: " << worker_tid << " has Detached" << std::endl;                                    
                //close(slave_socket);
				} 
            else{ // данные пришли от какого-то воркера, вычитываем их
                //std::cout << "Master PID: " << getpid() << "; Ivent From Worker!" << std::endl;
                    syslog(LOG_ERR, "Master: read_fd return NO Master Socket! %s", strerror(errno));
                }    				
			} // end for!
    } //  end while!
    

	return 0;
    }
