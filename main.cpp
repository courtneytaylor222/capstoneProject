

#include <iostream>
#include <boost/asio.hpp>         // Core Boost.Asio networking
#include <boost/asio/ssl.hpp>     // SSL/TLS support for Boost.Asio
#include <string>
#include <vector>
#include "network.hpp"
#include "ui.hpp"

constexpr const char* GREEN = "\033[32m";
constexpr const char* MAGENTA = "\033[35m";
constexpr const char* RESET = "\033[0m";

std::atomic<bool> running(true); 

void receive_loop(Connection& conn, const std::string& peerName){
    try{
        while(running){
            std::string incoming = conn.receiveMessage();
            if (incoming =="EXIT" || incoming == "exit"){
                std::cout << "\n" << peerName << " has left the chat. Type EXIT to leave chat too. \n";
                running = false;
                break;
            }
            std::cout << MAGENTA << peerName << ": " << incoming << RESET << std::endl;
            
        }
    } catch (std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        running = false;
    }
}

/* void send_messages(boost::asio::ip::tcp::socket& socket_){
    try{
        std::string message;
        while(connected){
            std::cout << "You: ";
            std::getLine(std::cin, message);
            if(message == "EXIT" || message == "exit"){
                socket.close();
                connected = false;
                break;
            }
            boost::asio::write(socket, boost::asio::buffer(message));
        }
    }catch(std::exception& e){
        std::cerr << "Error while sending: " << e.what() <<"\n";
    }
} */


int main()
{
    //step 1: show menu and get session details
    ChatSession session = menu();

    //Step 2: Set-up boost asio context
    boost::asio::io_context io_context;

    //Step 3: Set-up connection object
    Connection conn(io_context);

    //Step 4: Connect based on role
    if (session.getOption() == 1){
        //Server
        conn.listen(session.getPortNumber());
    } else{
        //client
        std::string ip;
        std::cout << "Enter the IP address to connect (usually 127.0.0.1 for local): ";
        std::cin >> ip;
        conn.connect(ip, session.getPortNumber());
    }

    //Exchange names between peers
    std::string myName = (session.getOption() == 1) ? session.getServerName() : session.getClientName();
    conn.sendMessage (myName + "\n"); //send my name

    std::string peerName = conn.receiveMessage();
    peerName.erase(peerName.find_last_not_of("\n")+1); //trim newline

    std::cout << "\n Connected with " << peerName << "! Type message below. Type EXIT to quit. \n";

    //Start receiver thread.
    std::thread receiver(receive_loop, std::ref(conn), peerName);

    //Main thread handles sending
    //will exit loop when user types EXIT

    while (running){
        //Outgoing message
        std::string outgoingMessage;
        std::cout << std::flush;
        std::getline(std::cin >> std::ws, outgoingMessage); //ignores whitespace

        if (outgoingMessage == "EXIT" || outgoingMessage == "exit"){
            std::cout << "Exiting chat... \n";
            conn.sendMessage("EXIT\n");

            boost::system::error_code ec;

            conn.getSocket().shutdown(ec);
            if(ec){
                std::cerr << "SSL shutdown failed: " << ec.what() << std::endl;
            }
            
            conn.getSocket().lowest_layer().close(ec);
            if(ec){
                std::cerr << "Socket close failed: " << ec.what() << std::endl;
            }

            running = false;
            break;
        }

        
        conn.sendMessage(outgoingMessage + "\n");

    }

    //Wait for receiver thread to finish
    if (receiver.joinable()){
        receiver.join();
    }


    //std::cout << "Exiting chat... \n";



    return 0;
}
