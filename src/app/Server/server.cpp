#include "server.hpp"

void CommRoom::joinRoom(NodePtr node) {
    this->nodes.insert(node);
}

void CommRoom::leaveRoom(NodePtr node) {
    this->nodes.erase(node);
}

void CommRoom::broadcast(NodePtr node, MessageProtocol &msg) {
    messageQueue.push(msg);
    while(!messageQueue.empty()){
        MessageProtocol message = messageQueue.front();
        messageQueue.pop();
        for(auto &currNode : nodes){
            if(currNode != node){
                currNode->write(message);
            }
        }
    }
}

void Session:async_read() {
    auto self(shared_from_this());
    boost::asio::async_read(clientSocket, buffer, "\n", [this, self](boost::system::error_code ec, size_t length){
        if(!ec){
            string data(boost::asio::buffers_begin(buffer.data()), boost::asio::buffers_end(buffer.data()) + length);
            buffer.consume(length);
            cout << "Received message: " << data << endl;
            MessageProtocol msg(data);
            send(msg);
            async_read();
        }
        else{
            room.leaveRoom(shared_from_this());
            if(ec != boost::asio::error::eof){
                cout << "Error: " << ec.message() << endl;
            }
            else {
                cout << "Connection closed by peer node!" << endl;
            }
        }
    });  
}

void Session::async_write(string message, size_t messageLength) {
    auto writeHandler = [&](boost::system::error_code ec, size_t length){
        if(!ec){
            cout << "Sent message: " << message << endl;
        }
        else{
            cout << "Write Error: " << ec.message() << endl;
        }
    };
    boost::asio::async_write(clientSocket, boost::asio::buffer(message, messageLength), writeHandler);
}

void Session::write(MessageProtocol &msg) {
    messageQueue.push(msg);
    while(!messageQueue.empty()){
        MessageProtocol message = messageQueue.front();
        messageQueue.pop();
        bool decodedHeader = message.decodeHeader();
        if(decodedHeader){
            string body = message.getBody();
            async_write(body, message.getBodyLength());
        }
        else{
            cout << "Error: Message too long" << endl;
        }
    }
}

void Session::send(MessageProtocol &msg){
    room.broadcast(shared_from_this(), msg);
}

void Session::start() {
    room.join(shared_from_this());
    async_read();
}

using boost:asio::ip::address_v4;

void accept_connection(boost::asio::io_context &io, char* port, tcp::acceptor &acceptor, CommRoom &room, const tcp::endpoint &endpoint) {
    tcp::socket socket(io);
    // non io blocking nature
    acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket){
        if(!ec){
            cout << "New connection from: " << socket.remote_endpoint().address().to_string() << endl;
            make_shared<Session>(move(socket), room)->start();
        }
        accept_connection(io, port, acceptor, room, endpoint);
    });
}

int main(int argc, char* argv[]){
    try{
        if(argc != 2){
            cerr << "Usage: ./commRoom <port>" << endl;
            return 1;
        }
        boost::asio::io_context io;
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
        tcp::acceptor acceptor(io, endpoint);
        CommRoom room;
        accept_connection(io, argv[1], acceptor, room, endpoint);
        io.run();
    }
    catch(exception &e){
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
