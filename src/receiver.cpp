#include "receiver.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <opentelemetry/proto/metrics/v1/metrics.grpc.pb.h> // Include your generated gRPC service header
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h> // Include your generated gRPC service header
class OtlpServiceImpl final : public ::opentelemetry::proto::collector::metrics::v1::MetricsService::Service {
public:
    ::grpc::Status Export(::grpc::ServerContext* context, const ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest* request, ::opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceResponse* response) override {
        // Handle incoming OTLP data
        std::cout << "Received OTLP data: " << request->DebugString() << std::endl;

        // Respond to the client
        // response->mutable_partial_success()->set_code(opentelemetry::proto::collector::metrics::v1::PartialSuccess::SUCCESS);
        // response->set_status(otlp::ExportResponse::SUCCESS);
        return grpc::Status::OK;
    }
};

OtlpGrpcReceiver::OtlpGrpcReceiver(const std::string& endpoint)
    : endpoint_(endpoint), running_(false) {
}

void OtlpGrpcReceiver::start() {
    if (running_) {
        return;
    }

    running_ = true;
    std::cout << "Starting OTLP gRPC receiver on " << endpoint_ << std::endl;

    // Create and start the gRPC server
    std::thread([this]() {
        OtlpServiceImpl service;

        grpc::ServerBuilder builder;
        builder.AddListeningPort(endpoint_, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "gRPC server is running on " << endpoint_ << std::endl;
        server->Wait(); // Block until the server shuts down

        std::cout << "gRPC server stopped" << std::endl;
        running_ = false;

    }).detach();
}

void OtlpGrpcReceiver::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    std::cout << "Stopping OTLP gRPC receiver on " << endpoint_ << std::endl;

    // In a real implementation:
    // 1. Shutdown the gRPC server
}

OtlpHttpReceiver::OtlpHttpReceiver(const std::string& endpoint)
    : endpoint_(endpoint), running_(false) {
}

void OtlpHttpReceiver::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    std::cout << "Starting OTLP HTTP receiver on " << endpoint_ << std::endl;
    
    // In a real implementation:
    // 1. Create and start an HTTP server
    // 2. Register the OTLP endpoints
    // 3. Handle incoming trace/metric data and pass to the next processor
}

void OtlpHttpReceiver::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    std::cout << "Stopping OTLP HTTP receiver on " << endpoint_ << std::endl;
    
    // In a real implementation:
    // 1. Shutdown the HTTP server
}
