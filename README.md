# Unofficial OTel Processor in CPP because why not

```
# prerequisite
git clone https://github.com/open-telemetry/opentelemetry-cpp.git
cd opentelemetry-cpp
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
sudo make install


```