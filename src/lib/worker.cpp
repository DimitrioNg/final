#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <sys/epoll.h> //epoll
#include <unistd.h> //fork
#include <sys/socket.h> //socketpair
#include <syslog.h> //syslog
#include <pthread.h>

#include "worker.h"
#include "fdtransceiver.h"
#include "unblock.h"
#include "webproc.h"

#define MAX_EPOLL_EVENTS 50
#define READ_BUFFER_SIZE 4096

/*
struct worker_args_struct{
    int socket_fd;
    std::string working_dir;
    }
*/

/*************************************************Секция: Воркера******************************************************/


int worker_rx_tx(int socket_fd, std::string serv_dir){

    struct epoll_event worker_epoll_event;
			    worker_epoll_event.data.fd = socket_fd;
			    worker_epoll_event.events = EPOLLIN; 

    static char read_buffer[READ_BUFFER_SIZE];
    memset(read_buffer, 0, sizeof(read_buffer)); // типо чистим буффер
    int recv_result = recv( socket_fd, //читаем данные из сокета
							read_buffer,
							READ_BUFFER_SIZE,
							MSG_NOSIGNAL
							);                     
    if(((recv_result==0) && (errno != EAGAIN))||(recv_result < 0)){ //ошибка чтения                   
       std::cout << "Worker TID: " << gettid() << " Socket Reading ERROR: recv_result = " << recv_result << std::endl;
        return -1; 
        }
        /*
        std::cout << "Worker PID: " << getpid() << ", TID: " << gettid() << "Close FD: " << socket_fd << std::endl;
		shutdown(socket_fd, SHUT_RDWR); // разрываем сокет
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, &worker_epoll_event) == -1){ // удаляем дескриптор из списка событий epoll
			std::cout << "Worker PID: " << getpid() << ", TID: " << gettid() << " Can not Delete Slave Socket Event For Socket: " << socket_fd << std::endl;
			}
        else std::cout <<  "Worker PID: " << getpid() << ", TID: " << gettid() << " Delete Slave Socket Event for Socket: " << socket_fd << "!!!" << std::endl;
			close(socket_fd); //и закрываем его
			  
        } 
        */
    else{ // приняли какие-то данные
		std::cout << "Worker TID: " << gettid() << " Received Data from " << socket_fd << " socket: " << read_buffer; // << std::endl; 
        send_msg(socket_fd, (std::string) &read_buffer[0], serv_dir);
        return 0;
        }          
    } //end worker worker_rx_tx


void* worker_tread(void* arg){
    std::cout << "Worker TID: " << gettid() << " Start!" << std::endl;
    //worker_args_struct* worker_str = (worker_args_struct*) arg;
    worker_args_struct* worker_str = (worker_args_struct*) arg;
    
    int socket_fd = worker_str->socket_fd;
    std::string working_dir = worker_str->working_dir;
    //sleep(5);
    //std::cout << "Чих! " << std::endl;
    std::cout << "Worker TID: " << gettid() << " Socket: " << socket_fd << ", Dir: " << working_dir << std::endl;
    
    set_nonblock(socket_fd); // делаем сокет неблокирующим

    int worker_epoll_fd =  epoll_create1(0);
	if (worker_epoll_fd == -1){
		std::cout << "Worker TID: " << gettid() << " Can't Open Epoll FD!!!" << std::endl;
		//return -1;
		}
    struct epoll_event worker_epoll_event;
		worker_epoll_event.data.fd = socket_fd;
		worker_epoll_event.events = EPOLLIN;
    
    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_ADD, socket_fd, &worker_epoll_event) == -1){ //регистрируем события от канала передачи сокетов от мастера к воркеру
		std::cout << "Worker TID: " << gettid() << " Can not registrate Slave Socket Event!!!" << std::endl;
				    //return -1;   
        }
    while(1){
        std::cout <<  "Worker TID: " << gettid() << " is Waiting for Epoll Event!" << std::endl;
        struct epoll_event worker_epoll_events[MAX_EPOLL_EVENTS];
        int N = epoll_wait	( // ждем сщбытий от Epoll
							worker_epoll_fd, //дескриптор
							worker_epoll_events, //список событий
							MAX_EPOLL_EVENTS, // максимальное количество возвращаямых событий
							-1 //таймаут (-1 ждать вечно)
							);
        for(unsigned int i = 0; i < N; i++){ // пробегаемся по нашим событиям
            if(worker_epoll_events[i].data.fd == socket_fd){
                std::cout << "Worker TID: " << gettid() << "; Ivent From Registered Socket FD: " << worker_epoll_events[i].data.fd << std::endl;
                int res = worker_rx_tx(worker_epoll_events[i].data.fd,  working_dir);
                if (res < 0){
                    shutdown(socket_fd, SHUT_RDWR); // разрываем сокет
                    close(socket_fd); //и закрываем его
                    close(worker_epoll_fd); // закрываем дескрипор epoll
                    std::cout << "Worker TID: " << gettid() << " has Closed!" << std::endl;
                    pthread_exit(0);
                    };
                }
            else{ // событие от прослушиваемого сокета
                std::cout << "Worker TID: " << gettid() << " Херня вышла!" << std::endl;
                std::cout << "Worker TID: " << gettid() << " has Closed!" << std::endl;
                pthread_exit(0);
                } // end else событие от прослушиваемого сокета   
            } //end for!
        }//end while(1)

        
    }

