#include <iostream>
#include <signal.h>
#include "collector.h"

static Collector* globalCollector = nullptr;

void signalHandler(int signum) {
    std::cout << "Received signal " << signum << std::endl;
    if (globalCollector) {
        globalCollector->shutdown();
    }
}

int main(int argc, char* argv[]) {
    std::string configPath = "config/config.yaml";
    
    // Allow custom config path
    if (argc > 1) {
        configPath = argv[1];
    }
    
    try {
        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // Create and start collector
        Collector collector(configPath);
        globalCollector = &collector;
        
        std::cout << "Starting OTLP collector..." << std::endl;
        collector.start();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
