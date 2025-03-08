#include <iostream>
#include "./Protocol/messageProtocol.hpp"
#include <set>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <boost/asio.hpp>
#include <queue>

using namespace std;

#ifndef COMMROOM_HPP
#define COMMROOM_HPP

using boost::asio::ip::tcp;

class Node {
public:
    virtual void send(MessageProtocol &msg) = 0;
    virtual void write(MessageProtocol &msg) = 0;
    virtual ~Node() = default;
};

typedef shared_ptr<Node> NodePtr;

class CommRoom {
public:
    void joinRoom(NodePtr node);
    void leaveRoom(NodePtr node);
    void broadcast(NodePtr node, MessageProtocol &msg);

private:
    queue<MessageProtocol> messageQueue;
    enum { MAX_NODES = 100 };
    set<NodePtr> nodes;
};

class Session: public Node, public enable_shared_from_this<Session> {
public:
    Session(tcp::socket sock, CommRoom &room);
    void start();
    void send(MessageProtocol &msg);
    void write(MessageProtocol &msg);
    void async_read();
    void async_write(string message, size_t messageLength);

private:
    tcp::socket clientSocket;
    boost::asio::streambuf buffer;
    CommRoom &room;
    queue<MessageProtocol> messageQueue;
};

#endif
