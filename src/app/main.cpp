#include <iostream>
#include "messageProtocol.hpp"
using namespace std;

int main() {
    string message;
    cout << "Enter a message: ";
    getline(cin, message);
    MessageProtocol msg(message);

    string fullMessage = msg.getData();
    cout << "Full message: " << fullMessage << endl;

    string body = msg.getBody();
    cout << "Body: " << body << endl;

    msg.decodeHeader();
    return 0;
}