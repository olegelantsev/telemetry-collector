#ifndef EXPORTER_H
#define EXPORTER_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>

class Exporter {
public:
    virtual ~Exporter() = default;
    
    virtual void exportTraces(const std::vector<opentelemetry::sdk::trace::SpanData>& spans) = 0;
    virtual void exportMetrics(const std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) = 0;
};

class OtlpExporter : public Exporter {
public:
    OtlpExporter(const std::string& endpoint);
    
    void exportTraces(const std::vector<opentelemetry::sdk::trace::SpanData>& spans) override;
    void exportMetrics(const std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) override;

private:
    std::string endpoint_;
    // In a real implementation, we'd have a gRPC client here
};

#endif // EXPORTER_H
