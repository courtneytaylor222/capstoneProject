#pragma once

#include <boost/asio.hpp>
//includes the main boost.asio library for networking
//gives access to classes like io_context, ip::tcp::socket, and ip::tcp::acceptor for TCP/IP communication

#include <boost/asio/ssl.hpp>
// handles ssl (secure sockets layer) support from boost.asio
//need this to enable encrypted connections

#include <string>

#include <memory> //for std::unique_ptr

enum class ConnectionRole{ Server, Client };


class Connection{   
public:
    Connection(boost::asio::io_context& io_context, ConnectionRole role);
    

    void connect(const std::string& ip, int port_number); 
    //Connect to a peer (client-side)
    //Tries to connect to another user by IP and port, used when your app is the client

    void listen(int port_number);
    //Used when your app is the server

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& getSocket();
    //returns a reference to the underlying SSL socket

    void sendMessage(const std::string& message);
    
    std::string receiveMessage();

private:
    boost::asio::io_context& io_context_;
    //store reference to io_context so we can access directly in functions in cpp

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::acceptor acceptor_;
    //socket_ is the communication channel
    //acceptor_ is to accept the incoming connections

    boost::asio::ssl::context ssl_context;
    std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ssl_socket;
    //ssl_context manages certificates, keys and SSL setup
    //ssl_socket wraps a TCP socket and handles encrypted communication


};
