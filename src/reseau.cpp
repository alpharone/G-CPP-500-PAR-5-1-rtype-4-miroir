#include "../includes/reseau.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

template<typename T>
client::network<T>::network()
{

}

template<typename T>
epoll_event client::network<T>::setnetword()
{

    struct sockaddr_in socket_address;
    struct epoll_event ev;
    
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == -1)
        exit(84);
    socket_address.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr);

    socket_address.sin_port = htons(atoi("8080"));
    connection = connect(socket_, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if (connection < 0) {
        exit(84);
    }
    epoll_file_ = epoll_create1(0);
    if (epoll_file_ == -1)
        exit(EXIT_FAILURE);
    ev.events = EPOLLIN;
    ev.data.fd = socket_;
    if (epoll_ctl(epoll_file_,
        EPOLL_CTL_ADD, socket_, &(ev)) == -1)
        exit(EXIT_FAILURE);
}

template<typename T>
uint32_t client::network<T>::new_player()
{
    write(socket_, "\x01",1);
}

template<typename T>
ssize_t client::network<T>::read_soket(T* buffer, size_t size)
{
    size_t total_read = 0;

    while (total_read <= size) {
        size_t num_read = read(socket_, (uint8_t*)buffer + total_read, size - total_read);
        if (num_read <= 0)
            return num_read;
        total_read += num_read;
    }
    return total_read;
}

template<typename T>
void client::network<T>::rec()
{
    uint16_t length;
    u_int32_t seq;
    header_t h;

    if (read_soket(&h, sizeof(h) <= 0))
        return;

    length = htons(h.length);
    seq = htonl(h.seq);
    std::vector<u_int8_t> obj(length);
    if (read_soket(&obj.data(), length) <= 0)
        return;
    auto it = code_function.find(static_cast<MessageType>(h.type));
    if (it != code_function.end())
        it->second();
}

template<typename T>
void client::network<T>::send(const Packet& p)
{
    std::vector<uint8_t> data = packet.serialize();
    write(sock, data.data(), data.size());
}