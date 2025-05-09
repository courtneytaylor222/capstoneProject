#include "network.hpp"
#include <iostream>

//implementing connection class
//defining the connection constructor
//initialisation list
Connection::Connection(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context)
    :socket_(io_context), acceptor_(io_context) , io_context_(io_context)
{
    //constructor bosy (if needed) can go here
}

//connect() implementation




//taking IP and port number as parameters

void Connection::connect(const std::string& ip, int port_number){
    boost::asio::ip::tcp::resolver resolver(io_context_);
    //creates a resolver object which is used to turn human-readable IP and port (like 127.0.0.1) into a machine-usable format called an endpoint
    //a resolver looks up IPs and gives you real addresses to connect to
    //resolver is a local variable in the function connect



}
