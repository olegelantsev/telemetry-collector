#include "receiver.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <opentelemetry/proto/metrics/v1/metrics.grpc.pb.h>
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// gRPC receiver (unchanged)
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
        : socket_(std::move(socket)) {}

    void start() { readRequest(); }

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;

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
            std::cout << "Received OTLP HTTP data: " << req_.body() << std::endl;
            http::response<http::string_body> res{http::status::ok, req_.version()};
            res.set(http::field::content_type, "application/json");
            res.body() = "{}";
            res.prepare_payload();
            auto self = shared_from_this();
            http::async_write(socket_, res,
                [self](beast::error_code ec, std::size_t) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                });
        } else {
            http::response<http::string_body> res{http::status::not_found, req_.version()};
            res.set(http::field::content_type, "text/plain");
            res.body() = "Not Found";
            res.prepare_payload();
            auto self = shared_from_this();
            http::async_write(socket_, res,
                [self](beast::error_code ec, std::size_t) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
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
            net::io_context ioc{1};
            tcp::acceptor acceptor{ioc, tcp::endpoint(net::ip::make_address(host), port)};
            std::cout << "HTTP server is running on " << host << ":" << port << std::endl;
            while (running_) {
                tcp::socket socket{ioc};
                acceptor.accept(socket);
                std::make_shared<OtlpHttpSession>(std::move(socket))->start();
            }
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
