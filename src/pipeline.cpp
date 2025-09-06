#include "pipeline.h"

#include <opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h>
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h>

namespace proto = opentelemetry::proto;

Pipeline::Pipeline(const std::string& name)
    : name_(name) {}

void Pipeline::addReceiver(std::shared_ptr<Receiver> receiver) {
    receivers_.push_back(receiver);
}

void Pipeline::addProcessor(std::shared_ptr<Processor> processor) {
    processors_.push_back(processor);
}

void Pipeline::addExporter(std::shared_ptr<Exporter> exporter) {
    exporters_.push_back(exporter);
}

void Pipeline::initialize() {
    // Wire up receivers to processors and processors to exporters
    for (auto& receiver : receivers_) {
        receiver->setNextProcessor(!processors_.empty() ? processors_.front() : nullptr);
    }

    for (size_t i = 0; i < processors_.size(); i++) {
        if (i + 1 < processors_.size()) {
            processors_[i]->addNext(processors_[i + 1]);
        } else {
            // Last processor connects to all exporters
            for (auto& exporter : exporters_) {
                processors_[i]->addNext(exporter);
            }
        }
    }
}

void Pipeline::start() {
    for (auto& receiver : receivers_) {
        receiver->start();
    }
}

void Pipeline::shutdown() {
    for (auto& receiver : receivers_) {
        receiver->stop();
    }
}