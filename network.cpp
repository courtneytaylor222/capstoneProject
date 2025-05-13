#include "network.hpp"
//brings in the header file with contains the declaration of the Connection class
//bringing in functions and members

#include <iostream>
//for cout and cerr

//implementing connection class
//defining the connection constructor
//initialisation list
//passing io_context by reference as it must be managed outside the class
//initialising a socket using same io context
//acceptor using same io context
//ssl_context_(...): Chooses TLS version (v1.2 here).
//ssl_socket_(...): Creates a socket that uses this SSL context.
//set_options(...): Adds common options to make SSL safer and compatible.
//use_certificate_chain_file(...): Loads your certificate (cert.pem).
//use_private_key_file(...): Loads your private key (key.pem) that proves you're the owner of the certificate.
Connection::Connection(boost::asio::io_context& io_context)
    :socket_(io_context),
    acceptor_(io_context) , 
    io_context_(io_context),
    ssl_context(boost::asio::ssl::context::tlsv12),
    ssl_socket(io_context, ssl_context)
{
    ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use
    );

    ssl_context.use_certificate_chain_file("cert.pem");
    ssl_context.use_private_key_file("key.pem", boost::asio::ssl::context::pem);

}

    //constructor body (if needed) can go here

    //getter for the socket
boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& Connection::getSocket(){
    return ssl_socket;
}

//connect() implementation, member function of Connection class
//create an endpoint object to specificy where to connect with ip and port number
//make_address(ip) converts 127.0.0.1 into an address object boost.asio can use
//client and server use same point number
//try to connect client socket to endpoint
//throw expection if server isn't running or IP/port is wrong with error message
void Connection::connect(const std::string& ip, int port_number){
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port_number);
    try{
        ssl_socket.lowest_layer().connect(endpoint);
        ssl_socket.handshake(boost::asio::ssl::stream_base::client);
        std::cout << "Connected to server at " << ip << " : " << port_number;
    }
    catch(std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
    }
}

//listen() implementation, member function of connection class.
//Server listen doesn't need IP and it uses all availables IPs by default
//listens on a specific port
//tcp::v4() tells boost to listen for IPv4 connections
//endpoint(listen for IPv4 connections - the protocol, on this specific port)
//opens the acceptor socket, allocates resources for the socket and configures it with the protocol specified by endpoint
//re-use address is useful as it allows the server to quickly rebind to a port recently used
//without this option, attempting to restart a server immediately after shutdown can result in 'address already in use' error
//OS prevents rebinding to the port until all previous connections are closed
//improves restart times and reliability
//then bind acceptor to the endpoint
//then listen for incoming TCP connection requests
//print listening message to the console
//program stops here until the client connects
//accept(socket_) accepts the connection into the socket


void Connection::listen(int port_number){
    try{
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_number);
        
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        std::cout << "listening on port " << port_number << "...\n";

        acceptor_.accept(ssl_socket.lowest_layer());
        std::cout << "Client connected!\n";

        ssl_socket.handshake(boost::asio::ssl::stream_base::server);
    }
    catch(std::exception& e){
        std::cerr << "Error while listening: " << e.what() << "\n";
    }
}

//send message implementation passing the message by const reference
//declare a boost error code object called ec
//if an error occurs during the send the object will hold the error info
//This avoids throwing exceptions and is used to check for success/failed manually
//write() is a blocking function for synchronous operations
//write has 3 parameters socket, buffer, error code message
void Connection::sendMessage(const std::string& message){
    boost::system::error_code ec;
    boost::asio::write(ssl_socket, boost::asio::buffer(message + "\n"), ec);

    if (ec){
        std::cerr << "Message failed to send: " << ec.message() << '\n' ;
    }
}

//receive message implementation returning a string, which will be the message received from the peer
//it's expected to use the socket_ already connected
//create a stream buffer named buf
//read from the socket into the buffer until it sees a newline character
//it's a blocking call - it will wait until a complete message is receive
//buf is where the received data will go
//attach the buffer to an input stream using istream input
//create an input stream that reads from the buffer
//use istream so that we can now easily use things like std::getline() to extract a line of text
//extract the message
//getline reads a full line up to and not including the \n from the input stream into the message string
//the \n that was read from the socket is discarded
//now message holds the full message sent by the peer
//the function returns the string so it can be displayed or processed by other parts of the program
//summary: message comes through the socket, place in a buffer, read it with the stream, message handed as a string

std::string Connection::receiveMessage(){
    boost::asio::streambuf buf;
    boost::asio::read_until(ssl_socket, buf, "\n");

    std::istream input(&buf);
    std::string message;
    std::getline(input, message);
    return message;
}



//taking IP and port number as parameters

//void Connection::connect(const std::string& ip, int port_number){
    //boost::asio::ip::tcp::resolver resolver(io_context_);
    //creates a resolver object which is used to turn human-readable IP and port (like 127.0.0.1) into a machine-usable format called an endpoint
    //a resolver looks up IPs and gives you real addresses to connect to
    //resolver is a local variable in the function connect



//}
