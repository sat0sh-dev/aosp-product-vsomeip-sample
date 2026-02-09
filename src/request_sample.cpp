/**
 * vsomeip Request Sample (Client)
 *
 * Phase 4: SOME/IP integration
 *
 * This sample demonstrates:
 * - Creating a vsomeip application
 * - Requesting a service
 * - Sending a request and receiving a response
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "sample_ids.hpp"

class RequestSample {
public:
    RequestSample() :
        app_(vsomeip::runtime::get()->create_application("vsomeip_request_sample")),
        running_(true) {
    }

    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize vsomeip application" << std::endl;
            return false;
        }

        app_->register_availability_handler(
            SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID,
            std::bind(&RequestSample::on_availability, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));

        app_->register_message_handler(
            SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID,
            std::bind(&RequestSample::on_response, this,
                      std::placeholders::_1));

        app_->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

        return true;
    }

    void start() {
        app_->start();
    }

    void stop() {
        running_ = false;
        app_->stop();
    }

private:
    void on_availability(vsomeip::service_t service,
                        vsomeip::instance_t instance,
                        bool is_available) {
        std::cout << "Service [" << std::hex << service << "."
                  << instance << "] is "
                  << (is_available ? "available" : "not available")
                  << std::endl;

        if (is_available) {
            send_request();
        }
    }

    void on_response(const std::shared_ptr<vsomeip::message>& response) {
        std::cout << "Received response from ["
                  << std::hex << response->get_service() << "."
                  << response->get_instance() << "]" << std::endl;

        auto payload = response->get_payload();
        std::cout << "Payload: ";
        for (uint32_t i = 0; i < payload->get_length(); i++) {
            std::cout << std::hex << (int)payload->get_data()[i] << " ";
        }
        std::cout << std::endl;
    }

    void send_request() {
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(SAMPLE_SERVICE_ID);
        request->set_instance(SAMPLE_INSTANCE_ID);
        request->set_method(SAMPLE_METHOD_ID);

        // Create payload
        auto payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data = {0x48, 0x45, 0x4C, 0x4C, 0x4F}; // "HELLO"
        payload->set_data(data);
        request->set_payload(payload);

        std::cout << "Sending request..." << std::endl;
        app_->send(request);
    }

    std::shared_ptr<vsomeip::application> app_;
    bool running_;
};

int main() {
    RequestSample sample;

    if (!sample.init()) {
        return 1;
    }

    std::cout << "Starting vsomeip request sample..." << std::endl;
    sample.start();

    return 0;
}
