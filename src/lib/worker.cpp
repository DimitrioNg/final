#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <sys/epoll.h> //epoll
#include <unistd.h> //fork
#include <sys/socket.h> //socketpair
#include <syslog.h> //syslog

#include "worker.h"
#include "fdtransceiver.h"
#include "unblock.h"
#include "webproc.h"

#define MAX_EPOLL_EVENTS 5
#define READ_BUFFER_SIZE 4096



/*************************************************Секция: Воркера******************************************************/

// Класс Workers проводит учет запущенных воркеров, используется контейнер std::map
// структура: файловый_дескриптор_параы_сокетов_для_передачи_сокета_клиета_воркеру -> структура описывающая воркера(PID воркера, счетчик обрабатываемых клиентских сокетов)

void Workers::add (int fd, int pid, std::string w_dir){
        workers_map[fd].descriptor_counter=0;
        workers_map[fd].PID = pid;
        workers_map[fd].worker_dir = w_dir;
    }

int Workers::del_by_pid (pid_t pid){
        std::map<int,worker>::iterator it = workers_map.begin();
        while(!(it==workers_map.end())) {  // перебираем список воркеров
            if(it->second.PID == pid){
                workers_map.erase(it);
                return it->first;
                }
            it++;
            }
        return -1;
        }

int Workers::inc (void){
        std::map<int,worker>::iterator it = workers_map.begin();
        if(it == --workers_map.end()){
            //std::cout << "Only One Element: ";
            first_att = false;
            return it->first; // если воркер зарегистрирован только один или обращение первое, то тут и думать нечего
            } 
        //it = workers_map.begin();    
        if(first_att){
            //std::cout << "First Attempt: ";
            first_att = false;
            last_fd = it->first;
            return it->first;
            }
        unsigned int max_counter  = 0;
        int current_fd = it->first;
        while(!(it==workers_map.end())) {  // перебираем воркеров
            if(it->second.descriptor_counter > max_counter){
                current_fd = it->first;
                max_counter = it->second.descriptor_counter;
                } 
            it++;
            } // end while
        if (current_fd==last_fd){
            it = workers_map.find(current_fd); //устанавливаем итератор на заданный ключ
            ++it;
            if (it == workers_map.end()) it = workers_map.begin();
                current_fd = it->first;
            }
        last_fd = current_fd; //запоминаем последний fd
        ++max_counter;
        workers_map[current_fd].descriptor_counter = max_counter;
        first_att = false;
        return current_fd;
        }

void Workers::dec (int fd){
        if((workers_map.find(fd) != workers_map.end()) && (workers_map.at(fd).descriptor_counter > 0)){
            --workers_map.at(fd).descriptor_counter;
            }
        }

void Workers::print (void){
        std::map<int,worker>::iterator it = workers_map.begin();
        while(!(it==workers_map.end())) {  // выводим их
        std::cout << "Worker FD: " << it->first << " -> FD Counter = " << it->second.descriptor_counter << ", PID = " << it->second.PID << ";" << std::endl;
        it++;
        }
    }

int Workers::del_by_fd (int fd){
        return workers_map.erase(fd);
        }  


int create_worker( int i,
                    int listen_fd, //прослушиваемый сокет
                    Workers &workers, // ссылка на объект с описанием воркеров
                    std::string worker_dir
                    ){ //создает воркера, возвращает PID воркера
    //std::cout << "Start Create Worker " << i << ";" << std::endl;
	pid_t	pid; 

    int		master_worker_sockfd[2];        //пара сокетов для передачи ческрипторов от мастера воркеру
	socketpair(AF_LOCAL, SOCK_STREAM, 0, master_worker_sockfd);
	if ((pid = fork()) > 0){                //pid = PID воркера и код мастера
		close(master_worker_sockfd[1]);         // закрываем один конец канала со стороны мастера
		workers.add(master_worker_sockfd[0],    //сохраняем дескриптор канала в который будем передавать файловый дескрптор от мастера воркеру
                    pid,                       //запоминаем PID воркера
                    worker_dir);
        return master_worker_sockfd[0];         //возвращаем файловый дескриптор, его нужно будет зарегистрировать в списке событий Epoll
		}
    else {
    // далее идет код дочернего процесса (воркера)
    dup2(master_worker_sockfd[1], STDERR_FILENO); //канал от воркера к мастеру, присоединяе к нему поток вывода ошибок
    close(master_worker_sockfd[0]); //закрываем открытые файловые дескрипторы канала
    close(master_worker_sockfd[1]); //закрываем открытые файловые дескрипторы канала
    close(listen_fd);               //файловый десриптор прослушиваемого сокета воркеру не нужен, поэтому закрываем его
                                    //сокет слушает и принимает мастер!!!


    // создаем дескриптор epoll для воркера
	int worker_epoll_fd =  epoll_create1(0);
	if (worker_epoll_fd == -1){
		std::cerr << "Worker PID: " << getpid() << " Can't Open Epoll FD!!!" << std::endl;
		//return -1;
		}
    //else std::cout << "Worker PID: " << getpid() << " Sucessful Open Epoll FD!!!" << std::endl;
    
    // Создать список для хранения возвращенных событий, возвращенных ждать 
    

    struct epoll_event worker_epoll_event;
			    worker_epoll_event.data.fd = STDERR_FILENO;
			    worker_epoll_event.events = EPOLLIN;

    

    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_ADD, STDERR_FILENO, &worker_epoll_event) == -1){ //регистрируем события от канала передачи сокетов от мастера к воркеру
				    std::cout << "Worker PID: " << getpid() << " Can not registrate Slave Socket Event!!!" << std::endl;
				    //return -1;   
                    }

    while(1){
        //std::cout <<  "Worker PID: " << getpid() << " is Waiting Epoll Event!" << std::endl;
        struct epoll_event worker_epoll_events[MAX_EPOLL_EVENTS];
            int N = epoll_wait	( // ждем сщбытий от Epoll
							worker_epoll_fd, //дескриптор
							worker_epoll_events, //список событий
							MAX_EPOLL_EVENTS, // максимальное количество возвращаямых событий
							-1 //таймаут (-1 ждать вечно) в нашем случае ждать вечно нельзя, потому что не будут приниматься файловые дескрипторы!
							);
            //std::cout << "epoll_wait Received: " << N << std::endl; 
        for(unsigned int i = 0; i < N; i++){ // пробегаемся по нашим событиям
            if(worker_epoll_events[i].data.fd == STDERR_FILENO){
                //std::cout << "Worker PID: " << getpid() << "; Ivent From Master!" << std::endl;
                int slave_socket;
                char buf[10];
                ssize_t n;
                n = sock_fd_read (  STDERR_FILENO, // читаем дескриптор через присоединенный ранее канал STDERR_FILENO 
                            buf, 
                            sizeof(buf), 
                            &slave_socket
                            );
                if (n <= 0){
                //err_quit("read_fd returned 0");
                //std::cout << "read_fd returned 0" << std::endl;
                    }
                else if(slave_socket < 0){
                    //err_quit("no descriptor from read_fd");
                    syslog(LOG_ERR, "Worker: No Descriptor From read_fd %s", strerror(errno));
                    //std::cout << "no descriptor from read_fd" << std::endl;
                    }
                else{
                    //сокет получен! 
                    set_nonblock(slave_socket); // делаем сокет неблокирующим
                    //std::cout << "Worker PID: " << getpid() << "; Read Result: " << n << "; Recieve Socket: " << slave_socket << std::endl;
			        worker_epoll_event.data.fd = slave_socket;
			        worker_epoll_event.events = EPOLLIN;
                //регистрируем его
                    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_ADD, slave_socket, &worker_epoll_event) == -1){
				        syslog(LOG_ERR, "Unable to Registrate Worker Socket Event for Worker %s", strerror(errno));
                        //std::cout << "Worker PID: " << getpid() << " Can not Registrate Slave Socket Event For Socket: " << slave_socket << std::endl;   
                        }
                    //else std::cout <<  "Worker PID: " << getpid() << "Registrate Slave Socket Event for Socket: " << slave_socket << "!!!" << std::endl;   
                    }
                } // end if(worker_epoll_events[i].data.fd == STDERR_FILENO)
            else{ // событие от прослушиваемого сокета
                //std::cout <<  "Worker PID: " << getpid() << "Event From Registered Slave Socket: " << worker_epoll_events[i].data.fd << "!!!" << std::endl;
                static char read_buffer[READ_BUFFER_SIZE];
                memset(read_buffer, 0, sizeof(read_buffer)); // типо чистим буффер
                int recv_result = recv	(   worker_epoll_events[i].data.fd,
											read_buffer,
											READ_BUFFER_SIZE,
											MSG_NOSIGNAL
											);

                //std::cout << "recv Result: " << recv_result << std::endl;                        
                if((recv_result==0) && (errno != EAGAIN)){ //ошибка чтения
					shutdown(worker_epoll_events[i].data.fd, SHUT_RDWR); // разрываем сокет
                    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_DEL, worker_epoll_events[i].data.fd, &worker_epoll_event) == -1){ // удаляем дескриптор из списка событий epoll
				        syslog(LOG_ERR, "Unable to Delete Slave Socket Event for Worker %s", strerror(errno));   
                        }
					close(worker_epoll_events[i].data.fd); //и закрываем его
					
                        //теперь нужно сообщить мастеру что на одно зарегистрируемое событие у этого воркера стало меньше.
                    send_report_to_master(STDERR_FILENO);
                    }
				else if(recv_result > 0){ // приняли какие-то данные
					//std::cout << "Received Data from " << worker_epoll_events[i].data.fd << " socket: " << read_buffer; // << std::endl; 
                    //void send_msg(int &fd, const std::string &request)
                    send_msg(worker_epoll_events[i].data.fd, read_buffer, worker_dir);
					}
                else if(recv_result < 0){
                    std::cout << "Reading error: recv_result < 0!" << std::endl;
                    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_DEL, worker_epoll_events[i].data.fd, &worker_epoll_event) == -1){ 
				        syslog(LOG_ERR, "Unable to Delete Slave Socket Event for Worker %s", strerror(errno));
                        }
                    //else std::cout <<  "Worker PID: " << getpid() << "Registrate Slave Socket Event for Socket: " << worker_epoll_events[i].data.fd << "!!!" << std::endl;   
                    }    
                }    
            } //end for!   
        }
    }
}
/******************************************Конец Секции: Создание Воркера********************************************/