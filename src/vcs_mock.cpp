/**
 * VCS Mock Server (vsomeip)
 *
 * Phase 4.2: PAL2VCS vsomeip integration
 *
 * This mock server simulates the VCS (Vehicle Control Service) running on CVC.
 * It receives control commands from PAL2VCS via SOME/IP and returns responses.
 *
 * Usage:
 *   ./vcs_mock
 *
 * Expected requests:
 *   Payload: "hvac.temperature=25" (control command)
 *
 * Response:
 *   Payload: "OK" or "ERROR:reason"
 */

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include "sample_ids.hpp"

class VcsMock {
public:
    VcsMock() :
        app_(vsomeip::runtime::get()->create_application("vcs_mock")),
        running_(true) {
    }

    bool init() {
        if (!app_->init()) {
            std::cerr << "Failed to initialize vsomeip application" << std::endl;
            return false;
        }

        // Register message handler for control commands
        app_->register_message_handler(
            SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID,
            std::bind(&VcsMock::on_control_request, this,
                      std::placeholders::_1));

        // Offer the VCS service
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
    void on_control_request(const std::shared_ptr<vsomeip::message>& request) {
        // Extract payload as string
        auto payload = request->get_payload();
        std::string command(
            reinterpret_cast<const char*>(payload->get_data()),
            payload->get_length()
        );

        std::cout << "========================================" << std::endl;
        std::cout << "[VCS Mock] Received control command" << std::endl;
        std::cout << "  From client: 0x" << std::hex << request->get_client() << std::dec << std::endl;
        std::cout << "  Command: " << command << std::endl;

        // Process command (mock implementation)
        std::string response_str = process_command(command);

        std::cout << "  Response: " << response_str << std::endl;
        std::cout << "========================================" << std::endl;

        // Create and send response
        auto response = vsomeip::runtime::get()->create_response(request);
        auto response_payload = vsomeip::runtime::get()->create_payload();

        std::vector<vsomeip::byte_t> response_data(
            response_str.begin(), response_str.end()
        );
        response_payload->set_data(response_data);
        response->set_payload(response_payload);

        app_->send(response);
    }

    std::string process_command(const std::string& command) {
        // Parse command: "key=value" format
        size_t eq_pos = command.find('=');
        if (eq_pos == std::string::npos) {
            return "ERROR:INVALID_FORMAT";
        }

        std::string key = command.substr(0, eq_pos);
        std::string value = command.substr(eq_pos + 1);

        // Mock command processing
        std::cout << "  [Processing] Key: " << key << ", Value: " << value << std::endl;

        // Simulate some known commands
        if (key == "hvac.temperature") {
            int temp = std::stoi(value);
            if (temp < 16 || temp > 30) {
                return "ERROR:TEMP_OUT_OF_RANGE";
            }
            std::cout << "  [HVAC] Setting temperature to " << temp << "°C" << std::endl;
            return "OK";
        }
        else if (key == "window.position") {
            int pos = std::stoi(value);
            if (pos < 0 || pos > 100) {
                return "ERROR:POSITION_OUT_OF_RANGE";
            }
            std::cout << "  [Window] Setting position to " << pos << "%" << std::endl;
            return "OK";
        }
        else if (key == "light.headlight") {
            if (value != "on" && value != "off") {
                return "ERROR:INVALID_VALUE";
            }
            std::cout << "  [Light] Headlight " << value << std::endl;
            return "OK";
        }
        else {
            // Unknown command - still accept for testing
            std::cout << "  [Unknown] Accepting command for testing" << std::endl;
            return "OK";
        }
    }

    std::shared_ptr<vsomeip::application> app_;
    bool running_;
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "VCS Mock Server (Phase 4.2)" << std::endl;
    std::cout << "========================================" << std::endl;

    VcsMock mock;

    if (!mock.init()) {
        std::cerr << "Failed to initialize VCS mock" << std::endl;
        return 1;
    }

    std::cout << "Offering service:" << std::endl;
    std::cout << "  Service ID:  0x" << std::hex << SAMPLE_SERVICE_ID << std::dec << std::endl;
    std::cout << "  Instance ID: 0x" << std::hex << SAMPLE_INSTANCE_ID << std::dec << std::endl;
    std::cout << "  Method ID:   0x" << std::hex << SAMPLE_METHOD_ID << std::dec << std::endl;
    std::cout << std::endl;
    std::cout << "Waiting for control commands from PAL2VCS..." << std::endl;
    std::cout << "========================================" << std::endl;

    mock.start();

    return 0;
}
