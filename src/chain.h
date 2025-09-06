#ifndef CHAIN_H
#define CHAIN_H

#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>

class Chain {
public:
    virtual void handleMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) = 0;
    virtual void handleTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) = 0;
    virtual ~Chain() = default;
};

#endif // CHAIN_H