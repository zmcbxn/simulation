#!/bin/bash
set -e # 에러 발생 시 즉시 중단

sudo apt-get remove -y libjsoncpp-dev
sudo apt-get update && sudo apt-get install -y libjsoncpp-dev


echo "🚀 [1/4] 필수 시스템 패키지 및 DB 라이브러리 설치 중..."
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake git pkg-config \
    libpq-dev libhiredis-dev libssl-dev zlib1g-dev uuid-dev \
    librdkafka-dev libboost-all-dev # 카프카 및 부스트 필수! [cite: 2026-02-11]

echo "🚀 [2/4] cppkafka 소스 빌드 및 설치 중..."
if [ ! -d "/tmp/cppkafka" ]; then
    git clone https://github.com/mfontanini/cppkafka.git /tmp/cppkafka
fi
cd /tmp/cppkafka && mkdir -p build && cd build
cmake .. -DCPPKAFKA_EXAMPLES=OFF -DCPPKAFKA_DISABLE_TESTS=ON
make -j$(nproc)
sudo make install
sudo ldconfig # 시스템 라이브러리 목록 갱신 [cite: 2026-02-11]

echo "🚀 [3/4] Drogon 소스 빌드 및 설치 중..."
if [ ! -d "/tmp/drogon" ]; then
    git clone https://github.com/drogonframework/drogon.git /tmp/drogon --recurse-submodules
fi
cd /tmp/drogon && mkdir -p build && cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig
echo "✅ Drogon 설치 완료!"

echo "🚀 [4/4] 최종 시스템 라이브러리 확인..."
sudo ldconfig
pkg-config --modversion rdkafka
echo "✨ 모든 환경 설정이 완료되었습니다!"
