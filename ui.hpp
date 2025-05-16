
#pragma once
#include <string>
#include <iostream>

class ChatSession {
private:
    std::string client_name;
    std::string server_name;
    int option;
    int port_number;

public:
    //constructor to intialise the session with option and port number
    ChatSession(int opt, int port) : option(opt), port_number(port){}

    //setters for client and server names
    void setClientName(const std::string& name){
        client_name = name;
    }

    void setServerName(const std::string& name){
        server_name = name;
    }

    //getters for client and server names
    std::string getClientName() const{
        return client_name;
    }

    std::string getServerName() const{
        return server_name;
    }

    //getters for port number and option 
    int getOption() const{
        return option;
    }

    int getPortNumber() const{
        return port_number;
    }

    //setter for port number
    void setPortNumber(int port){
        port_number = port;
    }
};

//Function that handles the menu and returns a filled ChatSession
ChatSession menu();
