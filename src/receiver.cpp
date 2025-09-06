#include "receiver.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <google/protobuf/util/json_util.h>
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h>
#include <opentelemetry/proto/metrics/v1/metrics.grpc.pb.h>
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class OtlpServiceImpl final : public ::opentelemetry::proto::collector::metrics::v1::MetricsService::Service {
public:
    ::grpc::Status Export(::grpc::ServerContext* context, const ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest* request, ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceResponse* response) override {
        std::cout << "Received OTLP gRPC data: " << request->DebugString() << std::endl;
        return grpc::Status::OK;
    }
};

OtlpGrpcReceiver::OtlpGrpcReceiver(const std::string& endpoint)
    : endpoint_(endpoint), running_(false) {}

void OtlpGrpcReceiver::start() {
    if (running_) return;
    running_ = true;
    std::cout << "Starting OTLP gRPC receiver on " << endpoint_ << std::endl;
    std::thread([this]() {
        OtlpServiceImpl service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(endpoint_, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "gRPC server is running on " << endpoint_ << std::endl;
        server->Wait();
        std::cout << "gRPC server stopped" << std::endl;
        running_ = false;
    }).detach();
}

void OtlpGrpcReceiver::stop() {
    if (!running_) return;
    running_ = false;
    std::cout << "Stopping OTLP gRPC receiver on " << endpoint_ << std::endl;
    // Proper shutdown logic should be implemented
}

// HTTP receiver using Boost.Asio/Beast
class OtlpHttpSession : public std::enable_shared_from_this<OtlpHttpSession> {
public:
    OtlpHttpSession(tcp::socket socket)
        : socket_(std::move(socket)) {
        std::cout << "New HTTP connection established from " 
                  << socket_.remote_endpoint().address().to_string() << std::endl;
    }

    void start() { readRequest(); }

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

    void readRequest() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, req_,
            [self](beast::error_code ec, std::size_t) {
                if (!ec)
                    self->handleRequest();
            });
    }

    void handleRequest() {
        if (req_.method() == http::verb::post && req_.target() == "/v1/metrics") {
            std::cout << "Received OTLP HTTP Protobuf data, size: " << req_.body().size() << std::endl;

            // Parse OTLP protobuf from binary payload
            opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest pbRequest;
            if (!pbRequest.ParseFromString(req_.body())) {
                std::cerr << "Failed to parse OTLP Protobuf payload" << std::endl;
            } else {
                // Print metric names
                for (const auto& resourceMetrics : pbRequest.resource_metrics()) {
                    for (const auto& scopeMetrics : resourceMetrics.scope_metrics()) {
                        for (const auto& metric : scopeMetrics.metrics()) {
                            std::cout << "Metric name: " << metric.name() << std::endl;
                        }
                    }
                }
            }

            res_ = {http::status::ok, req_.version()};
            res_.set(http::field::content_type, "application/json");
            res_.body() = "{}";
            res_.prepare_payload();
            auto self = shared_from_this();
            http::async_write(socket_, res_,
                [self](beast::error_code ec, std::size_t bytes_transferred) {
                    boost::ignore_unused(bytes_transferred);
                    beast::error_code ignore_ec;
                    self->socket_.shutdown(tcp::socket::shutdown_send, ignore_ec);
                });
        } else {
            res_ = {http::status::not_found, req_.version()};
            res_.set(http::field::content_type, "text/plain");
            res_.body() = "Not Found";
            res_.prepare_payload();
            auto self = shared_from_this();
            http::async_write(socket_, res_,
                [self](beast::error_code ec, std::size_t) {
                    beast::error_code ignore_ec;
                    self->socket_.shutdown(tcp::socket::shutdown_send, ignore_ec);
                });
        }
    }
};

OtlpHttpReceiver::OtlpHttpReceiver(const std::string& endpoint)
    : endpoint_(endpoint), running_(false) {}

void OtlpHttpReceiver::start() {
    if (running_) return;
    running_ = true;
    std::cout << "Starting OTLP HTTP receiver on " << endpoint_ << std::endl;

    // Parse endpoint (host:port)
    auto pos = endpoint_.find(':');
    std::string host = "0.0.0.0";
    unsigned short port = 4318;
    if (pos != std::string::npos) {
        host = endpoint_.substr(0, pos);
        port = static_cast<unsigned short>(std::stoi(endpoint_.substr(pos + 1)));
    }

    std::thread([this, host, port]() {
        try {
            net::io_context ioc;

            tcp::acceptor acceptor{ioc, tcp::endpoint(net::ip::make_address(host), port)};
            std::cout << "HTTP server is running on " << host << ":" << port << std::endl;

            // Accept connections asynchronously
            std::function<void()> do_accept;
            do_accept = [&]() {
                acceptor.async_accept([&](beast::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<OtlpHttpSession>(std::move(socket))->start();
                    }
                    if (running_) do_accept();
                });
            };
            do_accept();

            ioc.run(); // <--- This is required for async handlers to be called
        } catch (const std::exception& e) {
            std::cerr << "HTTP server error: " << e.what() << std::endl;
        }
        std::cout << "HTTP server stopped" << std::endl;
    }).detach();
}

void OtlpHttpReceiver::stop() {
    if (!running_) return;
    running_ = false;
    std::cout << "Stopping OTLP HTTP receiver on " << endpoint_ << std::endl;
    // Proper shutdown logic should be implemented
}
