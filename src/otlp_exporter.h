#ifndef OTLP_EXPORTER_H
#define OTLP_EXPORTER_H

#include <string>
#include <vector>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>
#include "exporter.h"

class OtlpExporter : public Exporter {
public:
    OtlpExporter(const std::string& endpoint);
    
    void exportTraces(const std::vector<opentelemetry::sdk::trace::SpanData>& spans) override;
    void exportMetrics(const std::vector<opentelemetry::sdk::metrics::MetricData>& metrics) override;

private:
    std::string endpoint_;
    // In a real implementation, we'd have a gRPC client here
};

#endif // OTLP_EXPORTER_H
