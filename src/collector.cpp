#include "collector.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <thread>
#include <chrono>

Collector::Collector(const std::string& configPath) 
    : running_(false) {
    if (!parseConfig(configPath)) {
        throw std::runtime_error("Failed to parse configuration file: " + configPath);
    }
}

Collector::~Collector() {
    shutdown();
}

bool Collector::parseConfig(const std::string& configPath) {
    try {
        YAML::Node config = YAML::LoadFile(configPath);
        
        // Parse receivers
        if (config["receivers"]) {
            auto receiverNodes = config["receivers"];
            for (const auto& receiver : receiverNodes) {
                std::string receiverType = receiver.first.as<std::string>();
                if (receiverType == "otlp") {
                    YAML::Node protocols = receiver.second["protocols"];
                    
                    // GRPC receiver
                    if (protocols["grpc"]) {
                        std::string endpoint = protocols["grpc"]["endpoint"].as<std::string>();
                        receivers_.push_back(std::make_unique<OtlpGrpcReceiver>(endpoint));
                    }
                    
                    // HTTP receiver
                    if (protocols["http"]) {
                        std::string endpoint = protocols["http"]["endpoint"].as<std::string>();
                        receivers_.push_back(std::make_unique<OtlpHttpReceiver>(endpoint));
                    }
                }
            }
        }
        
        // Parse processors
        if (config["processors"]) {
            auto processorNodes = config["processors"];
            for (const auto& processor : processorNodes) {
                std::string processorType = processor.first.as<std::string>();
                if (processorType == "batch") {
                    processors_.push_back(std::make_unique<BatchProcessor>());
                }
            }
        }
        
        // Parse exporters
        if (config["exporters"]) {
            auto exporterNodes = config["exporters"];
            for (const auto& exporter : exporterNodes) {
                std::string exporterType = exporter.first.as<std::string>();
                if (exporterType == "otlp") {
                    std::string endpoint = exporter.second["endpoint"].as<std::string>();
                    exporters_.push_back(std::make_unique<OtlpExporter>(endpoint));
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config: " << e.what() << std::endl;
        return false;
    }
}

void Collector::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    
    // Start all receivers
    for (auto& receiver : receivers_) {
        receiver->start();
    }
    
    std::cout << "Collector pipeline started" << std::endl;
    
    // Keep running until shutdown is called
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Collector::shutdown() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Stop all receivers
    for (auto& receiver : receivers_) {
        receiver->stop();
    }
    
    std::cout << "Collector pipeline stopped" << std::endl;
}