 #include <iostream>
 #include <sys/socket.h>    //struct msghdr, struct iovec
 #include <unistd.h>        //getpid
 
 /***********************************Секция: Передача/Прием Дескрипторов Сокетов**************************************/
ssize_t sock_fd_write   (   //передача дескриптора через сокет
                        int sock,       // дескриптор сокета, через который осуществляется передача  
                        void *buf,      // указатель на буффер
                        ssize_t buflen, // размер буфера
                        int fd          // файловый дескриптор передаваемого сокета (-1 - данные без дескриптора!)
                        ){ 
    ssize_t     size;
    
    struct msghdr   msg;
    struct iovec    iov;

    union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        }   cmsgu;
    
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) { // случай когда передается дескриптор сокета
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        printf ("passing fd %d\n", fd);
        *((int *) CMSG_DATA(cmsg)) = fd;
    } else { // случай когда передаются данные без дескриптора сокета
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        printf ("not passing fd\n");
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0){
        std::cout << "PID - "<< getpid() << ": Send Data Through Socket Pair ERROR!!!" << std::endl;
        //perror ("sendmsg");
        }
        
    return size;
}

ssize_t sock_fd_read    (
                            int sock,           //файловый дескриптор  сокета через который принимается сообщение
                            void *buf,          //указатель на приемный буфер
                            ssize_t bufsize,    //размер приемного буфера
                            int *fd             //файловый дескриптор сокета, который принимаем
                            ){
    ssize_t size;

    if (fd) { //случай когда мы приняли сокет
        struct msghdr   msg;
        struct iovec    iov;
        union   {
                struct cmsghdr  cmsghdr;
                char control[CMSG_SPACE(sizeof (int))];
                } cmsgu;

        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        
        //size = recvmsg (sock, &msg, MSG_PEEK);
        //std::cout << "PID: " << getpid() << "; PEEK Result: " << size << std::endl;
        size = recvmsg (sock, &msg, 0);
        if (size < 0) {
            std::cout << "PID - "<< getpid() << ": Receive Data Through Socket Pair ERROR!!!" << std::endl;
            //perror ("recvmsg");
            //exit(1);
            return size;
            }
            cmsg = CMSG_FIRSTHDR(&msg);
            if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))){
                if (cmsg->cmsg_level != SOL_SOCKET) {
                    fprintf (stderr, "invalid cmsg_level %d\n",
                    cmsg->cmsg_level);
                    exit(1);
                    }
                if (cmsg->cmsg_type != SCM_RIGHTS) {
                    fprintf (stderr, "invalid cmsg_type %d\n",
                    cmsg->cmsg_type);
                    exit(1);
                    }
                *fd = *((int *) CMSG_DATA(cmsg));
                printf ("received fd %d\n", *fd);
                } 
            else
                *fd = -1;
        } else { //случай когда мы приняли данные без сокета
            size = read (sock, buf, bufsize);
            if (size < 0) {
                perror("read");
                exit(1);
                }
            }
    return size;
    }
     
void send_fd_to_worker(int sock, int fd) { 
    ssize_t size;
    int i;
    char buf[] = "fd";
    std::cout << "Send Buffer: " << &buf[0] << " and FD = " << fd << std::endl;
    size = sock_fd_write(sock, buf, sizeof(buf), fd);
    std::cout << "PID = " << getpid() << " Wrote " << size << " bytes with FD: " << fd << std::endl;
    }

void send_report_to_master(int sock){
    ssize_t size;
    int fd = -1;
    char buf[] = "-1";
    std::cout << "PID = " << getpid() << "Send Buffer: " << &buf[0] << "; " << sizeof(buf) <<" bytes and FD = " << fd << std::endl;
    size = sock_fd_write(sock, buf, sizeof(buf), fd);
    std::cout << "PID = " << getpid() << " Wrote " << size << " bytes with FD = " << fd << "; and Result = " << size << std::endl;
    }

/*********************************Конец Секции: Передача/Прием Дескрипторов Сокетов**********************************/