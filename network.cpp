#include <openssl/err.h>
//handling and reporting OpenSSL errors, ERR_get_error(), check and print any OpenSSL errors after connection or handshake attempts.
#include "network.hpp" //includes header file
#include <iostream> //for cout and cerr
#include <boost/asio/ssl/context.hpp>
//Boost.Asio ssl::context class, which is used to configure SSL settings (protocol, certificates, etc.) for secure sockets
#include <openssl/ssl.h> //used for SSL_CTX_set_cipher_list()
#include <memory> //for std unique_ptr

Connection::Connection(boost::asio::io_context& io_context, ConnectionRole role)
    :socket_(io_context),
    acceptor_(io_context) , 
    io_context_(io_context),
    ssl_context(
        role == ConnectionRole::Server
            ? boost::asio::ssl::context::tlsv12_server
            : boost::asio::ssl::context::tlsv12_client)
    //ssl_socket(io_context, ssl_context) originally here, too early
{
    ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use
    );

    //only the server loads the certificate and key
    if(role == ConnectionRole::Server){
        ssl_context.use_certificate_chain_file("rsacert.pem");
        ssl_context.use_private_key_file("rsakey.pem", boost::asio::ssl::context::pem);
    }

    // Set cipher list using native_handle and OpenSSL API
    if (SSL_CTX_set_cipher_list(ssl_context.native_handle(),
        "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-SHA:AES128-SHA") != 1)
    {
        std::cerr << "Error setting cipher list!" << std::endl;
    }

    ssl_socket = std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(io_context, ssl_context);

    // Disable verification for self-signed certificates
    ssl_socket->set_verify_mode(boost::asio::ssl::verify_none);
}

//getter for the socket
boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& Connection::getSocket(){
    return *ssl_socket;
}

//try to connect client socket to endpoint
//throw expection if server isn't running or IP/port is wrong with error message
void Connection::connect(const std::string& ip, int port_number){
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port_number);
    try{
        ssl_socket->lowest_layer().connect(endpoint);
        ssl_socket->handshake(boost::asio::ssl::stream_base::client);
        std::cout << "SSL handshake complete. The following chat will be encrypted. Connected to server at " << ip << " : " << port_number;
    }
    catch(std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
    }

    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
        std::cerr << "OpenSSL error: " << ERR_error_string(err, nullptr) << std::endl;
    }
}

//Server listens on a specific port, accepts connection requests and performs a handshake.
void Connection::listen(int port_number){
    try{
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_number);
        
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        std::cout << "listening on port " << port_number << "...\n";

        acceptor_.accept(ssl_socket->lowest_layer());
        std::cout << "SSL handshake complete. The following chat will be encrypted. Client connected!\n";

        ssl_socket->handshake(boost::asio::ssl::stream_base::server);
    }
    catch(std::exception& e){
        std::cerr << "Error while listening: " << e.what() << "\n";
    }

    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
        std::cerr << "OpenSSL error: " << ERR_error_string(err, nullptr) << std::endl;
    }
}


//write() is a blocking function for synchronous operations.
void Connection::sendMessage(const std::string& message){
    boost::system::error_code ec;
    boost::asio::write(*ssl_socket, boost::asio::buffer(message + "\n"), ec);

    if (ec){
        std::cerr << "Message failed to send: " << ec.message() << '\n' ;
    }
}

//The message comes through the socket, placed in a buffer, read it with the stream, message handed as a string.
std::string Connection::receiveMessage(){
    boost::asio::streambuf buf;
    boost::asio::read_until(*ssl_socket, buf, "\n");

    std::istream input(&buf);
    std::string message;
    std::getline(input, message);
    return message;
}
