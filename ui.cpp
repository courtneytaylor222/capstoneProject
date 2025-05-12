#include "ui.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <limits>

ChatSession menu(){
    std::cout << "Welcome to myChat!" << '\n';
    std::cout << "The following conversations will be encrypted. " << '\n';
    std::cout << "Select one of the following options: \n";
    std::cout << "1. Enter as the server peer (first on). \n";
    std::cout << "2. Enter as the client peer (second on). \n";
    std::cout << "Otherwise, type EXIT to end the encrypted chat at any time. \n";
    
    std::string input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //cin.ignore(...) means ignore this thing in the input buffer
    // first parameter is the number of characters to ignore
    // ignore up to and including the second parameter is the delimeter character you specify
    //std::numeric_limits<> ::max() gives the largest possible value of the data type
    // std::streamsize type used by streams to represent sizes, a signed integer type
    // this means Ignore up to the maximum possible number of characters — effectively, everything in the input buffer.
    //up to and including the \n
    //if getLine() was used, it wouldn't return and empty string

    //Handles exit command
    //HELPER FUNCTION???????
    if (input == "EXIT" || input == "exit"){
        std::cout << "Exiting program... \n";
        std::exit(0);
    }
    //continues with following code if input is not exit

    int option;
    // initialising

    std::stringstream ss(input); 
    //allows you to treat a string like input and extract values from it
    //ie is it 1 or 2 or something else like "Hello"?
    //it creates a stringstream object named ss and it initialises it with the content of the input
    //eg if the input was 2 then ss.str() == "2"

    ss >> option;
    // >> is the stream extraction operator and when used with a stream it reads data from the stream and assigns to the provided variable
    // if data in ss can be converted into the int option, the extraction will succeed.
    //if data cannot be converted (the user wrote words) the stream will enter a failed state

    bool validConversionOption = !ss.fail();
    //checking state of ss using fail(), will equal true if it failed
    //!ss.fail() will be false if it failed
    //set bool equal to T or F - ie did it work? false if no 

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
    }
    else {
        session.setClientName(name);
    
    }
    std::cout << "Awaiting connection from peer..." << '\n';

    return session;
}


