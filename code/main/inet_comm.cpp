/**
 * inet_comm.cpp
 */
#include "inet_comm.hpp"

using namespace InetComm;

Connection::Connection(const char* ip, uint16_t port) {
    srv_ip_ = ip;
    port_ = port;
}

void Connection::open() {
    rx_buff = (char*)malloc(rx_len);
    if (rx_buff == NULL) {
        ESP_LOGE(TAG, "Cannot allocate buffer");
        return;
    }
    inet_pton(AF_INET, srv_ip_, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port_);
    
    xTaskCreate((TaskFunction_t)&Connection::connect_, "connect task", 4096, this, 5, &conn_task);
}

void Connection::connect_() {
    while (true) {
        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_ < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        }
        ESP_LOGI(TAG, "Connecting to %s:%d", srv_ip_, port_);
        int err = connect(socket_, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", err);
            close(socket_);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        connected_ = true;
        ESP_LOGI(TAG, "Successfully connected");

        xTaskCreate((TaskFunction_t)&Connection::recv_msg, "inet_recv", 4096, this, 5, &recv_task);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void Connection::send_msg(const std::string& msg) {
    if (connected_) {
        int err = send(socket_, msg.c_str(), msg.length(), 0);
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }
    }
    else {
        ESP_LOGE(TAG, "Error: not connected");
    }
}

void Connection::recv_msg() {
    int fail_ctr = 0;
    while (true) {
        int len = recv(socket_, rx_buff, rx_len - 1, 0);
        // Error occurred during receiving
        if (len < 0) {
            ESP_LOGW(TAG, "recv failed: errno %d", errno);
            if (++fail_ctr > 5) {
                ESP_LOGE(TAG, "connection lost");
                terminate();
                xTaskNotifyGive(conn_task);
                vTaskDelete(NULL);
            }
        }
        // Data received
        else {
            if (len < rx_len) {
                rx_buff[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, srv_ip_);
                ESP_LOGI(TAG, "%s", rx_buff);
                // Notify commander
                if (notif_q != NULL) {
                    xQueueSendToBack(*notif_q, &rx_buff, portMAX_DELAY);
                }
            }
            else {
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, srv_ip_);
            }
        }
    }
}

void Connection::terminate() {
    if (socket_ != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(socket_, 0);
        close(socket_);
        connected_ = false;
    }
}