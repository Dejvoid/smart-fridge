#ifndef INET_COMM_HPP_
#define INET_COMM_HPP_

#include "commander.hpp"

#include <sys/socket.h>
#include <esp_log.h>
#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace ConsoleCommander {
    class Commander;
};

namespace InetComm {

constexpr const char* TAG = "InetComm";

constexpr int rx_len = 128;

constexpr int keep_alive = 1;
constexpr int keep_idle = 1;
constexpr int keep_interval = 1;
constexpr int keep_count = 1;

class Connection {
    const char* srv_ip_;
    uint16_t port_; 
    bool connected_ = false;
    int socket_;
    char* rx_buff;
    struct sockaddr_in dest_addr;
    TaskHandle_t recv_task;
    TaskHandle_t conn_task;
    QueueHandle_t* notif_q = NULL;

    friend class ConsoleCommander::Commander;

    void connect_();
    void recv_msg();
public:
    Connection(const char* srv_ip, uint16_t port);
    void open();
    void send_msg(const std::string& msg);
    void terminate();
};

};

#endif