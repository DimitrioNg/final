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

void worker_rx(int socket_fd, int epoll_fd, worker_fd_fifo_struct &fd_fifo ){
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
    if((recv_result==0) && (errno != EAGAIN)){ //ошибка чтения                   
		shutdown(socket_fd, SHUT_RDWR); // разрываем сокет
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, &worker_epoll_event) == -1){ // удаляем дескриптор из списка событий epoll
            syslog(LOG_ERR, "Worker: Can not Delete Slave Socket Event For Socket: %s", strerror(errno));
			}
        //else std::cout <<  "Worker PID: " << getpid() << "Delete Slave Socket Event for Socket: " << socket_fd << "!!!" << std::endl;
			close(socket_fd); //и закрываем его
			//sleep(8);
            //теперь нужно сообщить мастеру что на одно зарегистрируемое событие у этого воркера стало меньше.
            send_report_to_master(STDERR_FILENO);
            
        }    
	else if(recv_result > 0){ // приняли какие-то данные
		//std::cout << "Received Data from " << socket_fd << " socket: " << read_buffer; // << std::endl; 
        //помещаем их в очередь
        std::pair <int, std::string> p;
        p.first = socket_fd; p.second = (std::string) &read_buffer[0];
        pthread_mutex_lock(&fd_fifo.fd_queue_lock);
        fd_fifo.fd_queue.push_back(p);
        pthread_mutex_unlock(&fd_fifo.fd_queue_lock);
		}
    else if(recv_result < 0){
        //std::cout << "Reading error: recv_result < 0!" << std::endl;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, &worker_epoll_event) == -1){ 
            syslog(LOG_ERR, "Worker: Can not Delete Slave Socket Event For Socket: %s", strerror(errno));
			//return -1;   
            }   
        }     
    }


void* worker_tx_thread(void* arg){
	worker_fd_fifo_struct *fd_fifo = (worker_fd_fifo_struct*)arg;
	while(1){
		pthread_mutex_lock(&fd_fifo->fd_queue_lock); //блокируем доступ к очеререди
		pthread_cond_wait(&fd_fifo->fd_queue_cond, &fd_fifo->fd_queue_lock);
		bool fd_queue_empty = fd_fifo->fd_queue.empty();
        std::string working_dir = fd_fifo->working_dir;
		pthread_mutex_unlock(&fd_fifo->fd_queue_lock); //разблокируем доступ к очеререди
	    while (!fd_queue_empty){ //очередь не пуста
		    pthread_mutex_lock(&fd_fifo->fd_queue_lock); //блокируем доступ к очеререди
		    std::pair<int, std::string> tmp_pair = fd_fifo->fd_queue.front(); ////забираем первую пару из очереди
		    fd_fifo->fd_queue.pop_front(); //удаляем ее из очереди
		    pthread_mutex_unlock(&fd_fifo->fd_queue_lock); //разблокируем доступ к очеререди
		    send_msg(tmp_pair.first, tmp_pair.second, working_dir); // обрабатываем первый элемент очереди
		    //проверяем  не пуста ли очередь?
		    pthread_mutex_lock(&fd_fifo->fd_queue_lock); //блокируем доступ к очеререди 
		    fd_queue_empty = fd_fifo->fd_queue.empty();
		    pthread_mutex_unlock(&fd_fifo->fd_queue_lock); //разблокируем доступ к очеререди
		    }
	//очередь оказалась пустой
		} 
	pthread_exit(0);
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
    worker_fd_fifo_struct fd_fifo;
    fd_fifo.working_dir = worker_dir;
    pthread_t worker_tx_tid;

    //инициализируем мьютекс, обслуживающий очередь
    int s_cond;
    s_cond = pthread_mutex_init(&fd_fifo.fd_queue_lock, NULL);
    if (s_cond != 0){
		syslog(LOG_ERR, "Worker: Mutex Creation ERROR! %s", strerror(errno));
		return -1;
		}
    
    //инициализируем условную переменную, обслуживающую очередь		
    s_cond = pthread_cond_init(&fd_fifo.fd_queue_cond, NULL);
    if (s_cond != 0){
        syslog(LOG_ERR, "Worker: Condition Variable Creation ERROR! %s", strerror(errno));
		return -1;
		}

    //создаем поток
    s_cond = pthread_create(&worker_tx_tid,
							NULL, //Атрибуты потока
							worker_tx_thread, //функция потока 
							&fd_fifo);
    if (s_cond != 0){
        syslog(LOG_ERR, "Worker: FD Transmitter Thread Creation ERROR! %s", strerror(errno));
		return -1;
		}

    // создаем дескриптор epoll для воркера
	int worker_epoll_fd =  epoll_create1(0);
	if (worker_epoll_fd == -1){
        syslog(LOG_ERR, "Worker: Can't Open Epoll FD! %s", strerror(errno));
		//return -1;
		}
    //else std::cout << "Worker PID: " << getpid() << " Sucessful Open Epoll FD!!!" << std::endl;
    
    // Создать список для хранения возвращенных событий, возвращенных ждать 
    

    struct epoll_event worker_epoll_event;
			    worker_epoll_event.data.fd = STDERR_FILENO;
			    worker_epoll_event.events = EPOLLIN;

    

    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_ADD, STDERR_FILENO, &worker_epoll_event) == -1){ //регистрируем события от канала передачи сокетов от мастера к воркеру
				    syslog(LOG_ERR, "Worker: Can not registrate Slave Socket Event! %s", strerror(errno));
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
			        worker_epoll_event.data.fd = slave_socket;
			        worker_epoll_event.events = EPOLLIN;
                //регистрируем его
                    if (epoll_ctl(worker_epoll_fd, EPOLL_CTL_ADD, slave_socket, &worker_epoll_event) == -1){
				        syslog(LOG_ERR, "Unable to Registrate Worker Socket Event for Worker %s", strerror(errno));   
                        }   
                    }
                } // end if(worker_epoll_events[i].data.fd == STDERR_FILENO)
            else{ // событие от прослушиваемого сокета
                //запускаем приемник сообщений от сокета
                worker_rx(worker_epoll_events[i].data.fd, worker_epoll_fd, fd_fifo); //ставим прочитанные данные в очередь
                //посылаем сигнал потоку передающему обработанное сообщение в сокет
                pthread_mutex_lock(&fd_fifo.fd_queue_lock);
                pthread_cond_signal(&fd_fifo.fd_queue_cond);
                pthread_mutex_unlock(&fd_fifo.fd_queue_lock);  
                } //end событие от прослушиваемого сокета
            } //end for!   
        }
    }
}
/******************************************Конец Секции: Создание Воркера********************************************/