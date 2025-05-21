

#include <iostream>
#include <boost/asio.hpp>         // Core Boost.Asio networking
#include <boost/asio/ssl.hpp>     // SSL/TLS support for Boost.Asio
#include <string>
#include <vector>
#include "network.hpp"
#include "ui.hpp"

constexpr const char* MAGENTA = "\033[35m";
constexpr const char* RESET = "\033[0m";

std::atomic<bool> running(true); 

void receive_loop(Connection& conn, const std::string& peerName){
    try{
        while(running){
            std::string incoming = conn.receiveMessage();
            if (incoming =="EXIT" || incoming == "exit"){
                std::cout << "\n" << peerName << " has left the chat. Both peers must type EXIT for the connection to close. \n";
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

int main()
{
    //step 1: show menu and get session details
    ChatSession session = menu();

    //Step 2: Set-up boost asio context
    boost::asio::io_context io_context;

    //Step 3: Set-up connection object
    Connection conn(io_context, session.getOption() == 1 ? ConnectionRole::Server : ConnectionRole::Client);

    //Step 4: Connect based on role
    if (session.getOption() == 1){
        //Server
        conn.listen(session.getPortNumber());

        //receive the client's name first
        std::string client_name = conn.receiveMessage();
        session.setClientName(client_name);

        //send server name second
        conn.sendMessage(session.getServerName());
    } else{
        //client
        std::string ip;
        std::cout << "Enter the IP address to connect (usually 127.0.0.1 for local): ";
        std::cin >> ip;

        conn.connect(ip, session.getPortNumber());

        //send client name first
        conn.sendMessage(session.getClientName());
        
        //receive server name second
        std::string server_name = conn.receiveMessage();
        session.setServerName(server_name);
    }

    //Start receiver thread.
    std::string peerName = (session.getOption() == 1) ? session.getClientName() : session.getServerName();
    
    std::thread receiver(receive_loop, std::ref(conn), peerName);

    std::cout << "\n Connected with " << peerName << "! Type message below. Type EXIT to quit. \n";

    //Main thread handles sending
    while (running){
        //Outgoing message
        std::string outgoingMessage;
        std::cout << std::flush;
        std::getline(std::cin >> std::ws, outgoingMessage); //ignores whitespace

        if (outgoingMessage == "EXIT" || outgoingMessage == "exit"){
            std::cout << "Exiting chat... \n";
            conn.sendMessage("EXIT\n");
            running = false;
            break;
        }
        
        conn.sendMessage(outgoingMessage);

    }

    //Wait for receiver thread to finish
    if (receiver.joinable()){
        receiver.join();
    }

    std::cout << "Chat ended. Goodbye! \n";
    return 0;
}
