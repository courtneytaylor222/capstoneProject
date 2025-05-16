#include "ui.hpp"
#include <iostream>
#include <string>
#include <cstdlib> //for std::exit
#include <sstream> //for std::stringstream
#include <limits> //for std::numeric_limits

ChatSession menu(){
    std::cout << "Welcome to myChat!" << '\n';
    std::cout << "The following conversations will be encrypted. " << '\n';
    std::cout << "Select one of the following options: \n";
    std::cout << "1. Enter as the server peer (first on). \n";
    std::cout << "2. Enter as the client peer (second on). \n";
    std::cout << "Otherwise, type EXIT to end the encrypted chat at any time. \n";
    
    std::string input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //clearing input buffer
    
    if (input == "EXIT" || input == "exit"){
        std::cout << "Exiting program... \n";
        std::exit(0);
    }
    //continues with following code if input is not exit

    int option;
    std::stringstream ss(input); 
    ss >> option;
    //convert string input to an integer and setting option equal to the integer

    bool validConversionOption = !ss.fail();
    //checking if the conversion succeeded

    if (!validConversionOption || (option != 1 && option != 2)) {
        std::cout << "Invalid input. Exiting...\n";
        std::exit(1);
    }
    //if conversion was false or the option was not either 1 or 2, exit

    //if we get to here, the conversion worked
    std::cout << "Enter your name: \n";
    std::string name;
    std::cin >> name;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Please enter a port number between 1024-49151." <<'\n';
    std::cout << "Port number: \n";
    std::string port_input;
    std::cin >> port_input;
    //name as input because it may not be a valid number
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (port_input == "EXIT" || port_input == "exit"){
        std::cout << "Exiting program... \n";
        std::exit(0);
    }

    int port_number;
    std::stringstream port_ss(port_input);
    port_ss >> port_number;
    bool validConversionPort = !port_ss.fail();

    if (!validConversionPort || port_number < 1024 || port_number > 49151){
        std::cout << "Invalid port number. Exiting... \n";
        std::exit(1);
    }

    //create a session object
    ChatSession session(option, port_number);

    if (option ==1){
        session.setServerName(name);
        std::cout << "Awaiting connection from peer..." << '\n';
    }
    else {
        session.setClientName(name);
    
    }

    return session;
}


