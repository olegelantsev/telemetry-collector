#include "collector.h"
#include "pipeline.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include "batch_processor.h"
#include "otlp_exporter.h"
#include "processor.h"
#include "receiver.h"
#include "exporter.h"

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
        if (!std::filesystem::exists(configPath)) {
            std::cerr << "Configuration file does not exist: " << std::filesystem::absolute(configPath) << std::endl;
            return false;
        }

        YAML::Node config = YAML::LoadFile(configPath);

        // Parse pipelines
        if (!config["service"]) {
            std::cerr << "No service section in config" << std::endl;
            return false;
        }
        auto service = config["service"];
        if (!service["pipelines"]) {
            std::cerr << "No pipelines defined in services section" << std::endl;
            return false;
        }
        if (service["pipelines"]) {
            auto pipelineNodes = service["pipelines"];
            for (const auto& pipelineNode : pipelineNodes) {
                std::string pipelineName = pipelineNode.first.as<std::string>();
                YAML::Node pipelineConfig = pipelineNode.second;

                auto pipeline = std::make_unique<Pipeline>(pipelineName);

                // Receivers
                if (pipelineConfig["receivers"]) {
                    for (const auto& receiver : pipelineConfig["receivers"]) {
                        std::string receiverType = receiver.as<std::string>();
                        if (config["receivers"] && config["receivers"][receiverType]) {
                            YAML::Node receiverDef = config["receivers"][receiverType];
                            if (receiverDef["protocols"]) {
                                YAML::Node protocols = receiverDef["protocols"];
                                if (protocols["grpc"]) {
                                    std::string endpoint = protocols["grpc"]["endpoint"].as<std::string>();
                                    pipeline->addReceiver(std::make_shared<OtlpGrpcReceiver>(endpoint));
                                }
                                if (protocols["http"]) {
                                    std::string endpoint = protocols["http"]["endpoint"].as<std::string>();
                                    pipeline->addReceiver(std::make_shared<OtlpHttpReceiver>(endpoint));
                                }
                            }
                        }
                    }
                }

                // Processors
                if (pipelineConfig["processors"]) {
                    for (const auto& processor : pipelineConfig["processors"]) {
                        std::string processorType = processor.as<std::string>();
                        if (config["processors"] && config["processors"][processorType]) {
                            // Example: only batch processor supported
                            if (processorType == "batch") {
                                pipeline->addProcessor(std::make_shared<BatchProcessor>());
                            }
                        }
                    }
                }

                // Exporters
                if (pipelineConfig["exporters"]) {
                    for (const auto& exporter : pipelineConfig["exporters"]) {
                        std::string exporterType = exporter.as<std::string>();
                        if (config["exporters"] && config["exporters"][exporterType]) {
                            YAML::Node exporterDef = config["exporters"][exporterType];
                            if (exporterType == "otlp") {
                                std::string endpoint = exporterDef["endpoint"].as<std::string>();
                                pipeline->addExporter(std::make_shared<OtlpExporter>(endpoint));
                            }
                        }
                    }
                }

                pipeline->initialize();
                pipelines_.push_back(std::move(pipeline));
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

    // Start all pipelines
    for (auto& pipeline : pipelines_) {
        pipeline->start();
    }

    std::cout << "Collector pipeline(s) started" << std::endl;

    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Collector::shutdown() {
    if (!running_) {
        return;
    }
    running_ = false;

    // Stop all pipelines
    for (auto& pipeline : pipelines_) {
        pipeline->shutdown();
    }

    std::cout << "Collector pipeline(s) stopped" << std::endl;
}