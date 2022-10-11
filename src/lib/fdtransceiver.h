ssize_t sock_fd_write   (   //передача дескриптора через сокет
                        int sock,       // дескриптор сокета, через который осуществляется передача  
                        void *buf,      // указатель на буффер
                        ssize_t buflen, // размер буфера
                        int fd          // файловый дескриптор передаваемого сокета (-1 - данные без дескриптора!)
                        );

ssize_t sock_fd_read    (
                            int sock,           //файловый дескриптор  сокета через который принимается сообщение
                            void *buf,          //указатель на приемный буфер
                            ssize_t bufsize,    //размер приемного буфера
                            int *fd             //файловый дескриптор сокета, который принимаем
                            );

void send_fd_to_worker(int sock, int fd);

void send_report_to_master(int sock);

void send_msg(int, char*);