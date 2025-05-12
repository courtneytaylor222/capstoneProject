#include <boost/asio.hpp>
//includes the main boot.asio library for networking
//gives access to classes like io_context, ip::tcp::socket, and ip::tcp::acceptor for TCP/IP communication

#include <boost/asio/ssl.hpp>
// handles ssl (secure sockets layer) support from boost.asio
//need this to enable encrypted connections using boost

#include <string>

class Connection{
//functions will be called by other parts of the program eg main()    
public:
    // Constructor
    Connection(boost::asio::io_context& io_context);
    

    //Connect to a peer (client-side)
    void connect(const std::string& ip, int port_number); 
    //function declaration of the connection taking ip address by const reference and port as parameters
    //Tries to connect to another user by IP and port
    //used when your app is the client

    void listen(int port_number);
    //Used when your app is the server

    boost::asio::ip::tcp::socket& getSocket();
    //returns the actual socket so other parts of the program (SSL) can use it

    void sendMessage(const std::string& message);
    
    std::string receiveMessage();

private:
    boost::asio::io_context& io_context_;
    //store reference to io_context so we can access directly in functions in cpp

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::acceptor acceptor_;
    // socket_ is the communication channel
    //acceptor_ is to accept the incoming connections

};
