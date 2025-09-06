#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <memory>
// #include <variant>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>

#include "chain.h"

class Processor;
class Exporter;

// Variant type for supported telemetry data
// using TelemetryData = std::variant<
//     std::vector<opentelemetry::sdk::trace::SpanData>,
//     std::vector<opentelemetry::sdk::metrics::MetricData>
// >;

class Processor : public Chain {
public:
    virtual ~Processor() = default;

    // Set the next processor or exporter in the chain
    void addNext(std::shared_ptr<Chain> next) {
        nexts_.push_back(next);
    }

protected:
    // Forward processed data to the next in the chain
    void forwardTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) {
        for (auto& next : nexts_) {
            next->handleTraces(spans);
        }
    }

    void forwardMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) {
        for (auto& next : nexts_) {
            next->handleMetrics(metrics);
        }
    }
private:
    std::vector<std::shared_ptr<Chain>> nexts_;
};

#endif // PROCESSOR_H
