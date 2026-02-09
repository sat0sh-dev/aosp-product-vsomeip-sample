/**
 * vsomeip Response Sample (Server)
 *
 * Phase 4: SOME/IP integration
 *
 * This sample demonstrates:
 * - Creating a vsomeip application
 * - Offering a service
 * - Receiving requests and sending responses
 */

// TODO: Enable after vsomeip is built
#if 0

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include "sample_ids.hpp"

class ResponseSample {
public:
    ResponseSample() :
        app_(vsomeip::runtime::get()->create_application("vsomeip_response_sample")),
        running_(true) {
    }

    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize vsomeip application" << std::endl;
            return false;
        }

        app_->register_message_handler(
            SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID,
            std::bind(&ResponseSample::on_request, this,
                      std::placeholders::_1));

        app_->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

        return true;
    }

    void start() {
        app_->start();
    }

    void stop() {
        running_ = false;
        app_->stop_offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
        app_->stop();
    }

private:
    void on_request(const std::shared_ptr<vsomeip::message>& request) {
        std::cout << "Received request from ["
                  << std::hex << request->get_client() << "]" << std::endl;

        auto payload = request->get_payload();
        std::cout << "Payload: ";
        for (uint32_t i = 0; i < payload->get_length(); i++) {
            std::cout << std::hex << (int)payload->get_data()[i] << " ";
        }
        std::cout << std::endl;

        // Create response
        auto response = vsomeip::runtime::get()->create_response(request);

        // Create response payload
        auto response_payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data = {0x57, 0x4F, 0x52, 0x4C, 0x44}; // "WORLD"
        response_payload->set_data(data);
        response->set_payload(response_payload);

        std::cout << "Sending response..." << std::endl;
        app_->send(response);
    }

    std::shared_ptr<vsomeip::application> app_;
    bool running_;
};

int main() {
    ResponseSample sample;

    if (!sample.init()) {
        return 1;
    }

    std::cout << "Starting vsomeip response sample (server)..." << std::endl;
    std::cout << "Offering service [0x" << std::hex << SAMPLE_SERVICE_ID
              << ".0x" << SAMPLE_INSTANCE_ID << "]" << std::endl;

    sample.start();

    return 0;
}

#else

#include <iostream>

int main() {
    std::cout << "vsomeip_response_sample: vsomeip not yet built" << std::endl;
    std::cout << "Build Phase 4 libraries first:" << std::endl;
    std::cout << "  1. external/boost" << std::endl;
    std::cout << "  2. external/vsomeip" << std::endl;
    return 1;
}

#endif
