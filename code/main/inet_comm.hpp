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

constexpr const char* srv_ip = "192.168.1.112";
constexpr uint16_t port = 666;

constexpr int rx_len = 128;

constexpr int keep_alive = 1;
constexpr int keep_idle = 1;
constexpr int keep_interval = 1;
constexpr int keep_count = 1;

class Connection {
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
    void open();
    void send_msg(const std::string& msg);
    void terminate();
};

};

#endif