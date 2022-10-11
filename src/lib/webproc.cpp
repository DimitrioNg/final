#include <sstream>
#include <iostream>
#include <sys/socket.h> //send

//std::string serv_dir{};

/*********************************Секция: Обработка WEB (GET) запросов для воркера***********************************/

std::string parse_request(const std::string &for_parse) {

    std::size_t pos1 = for_parse.find("GET /");
    std::size_t pos2 = for_parse.find(" HTTP/1");
    if (pos1 == std::string::npos || pos2 == std::string::npos) return "";
    std::string ind = for_parse.substr(pos1 + 5, pos2 - pos1 - 5);
    if (ind.size() == 0) return "index.html";

    auto pos = ind.find('?');
    if (pos == std::string::npos)
        return ind;
    else
        return ind.substr(0, pos);
}

std::string http_error_404() {
    std::stringstream ss;
    // Create a result with "HTTP/1.0 404 NOT FOUND"
    ss << "HTTP/1.0 404 NOT FOUND";
    ss << "\r\n";
    ss << "Content-length: 0";
    ss << "\r\n";
    ss << "Content-Type: text/html";
    ss << "\r\n\r\n";
    return ss.str();
}

std::string http_ok_200(const std::string &data) {
    std::stringstream ss;
    ss << "HTTP/1.0 200 OK";
    ss << "\r\n";
    ss << "Content-length: ";
    ss << data.size();
    ss << "\r\n";
    ss << "Content-Type: text/html";
    ss << "\r\n";
    // ss << "Connection: close\r\n";
    ss << "\r\n";
    ss << data;
    return ss.str();
}


//void send_msg(int &fd, const std::string &request) {
void send_msg(int fd, char request[], std::string serv_dir){
    std::string str = request;
    std::cout << str;
    std::string f_name = parse_request(str);
    /**/
    if (f_name == "") {
        std::string err = http_error_404();
        send(fd, err.c_str(), err.length() + 1, MSG_NOSIGNAL);
        return;
    	} 
	else {
        std::stringstream ss;
        ss << serv_dir;
        if (serv_dir.length() > 0 && serv_dir[serv_dir.length() - 1] != '/') ss << "/";
        ss << f_name;
		FILE *file_in = fopen(ss.str().c_str(), "r");
        char arr[1024];
        if (file_in) {
            std::stringstream ss;
            std::string tmp_str;
            char c = '\0';
            while ((c = fgetc(file_in)) != EOF) {
                ss << c;
            	}
            tmp_str = ss.str();
            std::string ok = http_ok_200(tmp_str);
            send(fd, ok.c_str(), ok.size(), MSG_NOSIGNAL);
            fclose(file_in);
        	} 
		else {
            std::string err = http_error_404();
            send(fd, err.c_str(), err.size(), MSG_NOSIGNAL);
        	}

    	}
        
	}    
/******************************Конец Секции: Обработка WEB (GET) запросов для воркера********************************/