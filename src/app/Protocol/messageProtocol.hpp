#include <iostream>
#include <string>
#include <string.h>
using namespace std;
#ifndef MESSAGEPROTOCOL_HPP
#define MESSAGEPROTOCOL_HPP

// Predefined constants for the message protocol
enum { 
    HEADER = 4, // Size of the header in bytes
    MAX_BYTES = 512  // Maximum size of the body in bytes
};

class MessageProtocol {
private:
    char data[HEADER+MAX_BYTES];
    size_t BODY_LENGTH;

public:
    MessageProtocol() : BODY_LENGTH(0) {}

    MessageProtocol(string message){
        BODY_LENGTH = getBodyLength(message.size());
        encodeHeader();
        memcpy(data+HEADER, message.c_str(), BODY_LENGTH);
    }

    size_t getBodyLength(size_t messageLength){
        if(messageLength > MAX_BYTES) return MAX_BYTES;
        return messageLength;
    }

    // Get the whole data of the message
    string getData(){
        int length = HEADER + BODY_LENGTH;
        return string(data, length);
    }

    // Get the body of the message
    string getBody(){
        string data = getData();
        return data.substr(HEADER, BODY_LENGTH);
    }

    size_t getBodyLength(){
        return BODY_LENGTH;
    }

    void encodeHeader(){
        char currHeader[HEADER+1] = "";
        sprintf(currHeader, "%4d", static_cast<int>(BODY_LENGTH));
        // cout << "Encoded header: ";
        // for(int i=0; i<HEADER; i++) cout << currHeader[i];
        // cout << endl;
        memcpy(data, currHeader, HEADER);
    }

    bool decodeHeader(){
        char currHeader[HEADER+1] = "";
        memcpy(currHeader, data, HEADER);
        currHeader[HEADER] = '\0';
        // cout << "Decoded header: ";
        // for(int i=0; i<HEADER; i++) cout << currHeader[i];
        // cout << endl;
        int headerValue = atoi(currHeader);
        //cout << "Header value: " << headerValue << endl;
        if(headerValue > MAX_BYTES){
            BODY_LENGTH = 0;
            return false;
        }
        BODY_LENGTH = headerValue;
        return true;
    }

};

#endif 