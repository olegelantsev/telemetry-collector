#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>

class Processor {
public:
    virtual ~Processor() = default;
    
    virtual void processTraces(std::vector<opentelemetry::sdk::trace::SpanData>& spans) = 0;
    virtual void processMetrics(std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) = 0;
};

#endif // PROCESSOR_H
