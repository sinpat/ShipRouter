FROM ubuntu:rolling as build
LABEL description="Build container - ShipRouter"

RUN apt update && apt -y install \
    cmake gcc-9 g++-9 libssl-dev \
    make binutils build-essential git \
    zlib1g wget zlib cmake

RUN update-alternatives --install /usr/bin/gcc \
    gcc /usr/bin/gcc-9 80 --slave /usr/bin/g++ \
    g++ /usr/bin/g++-9 --slave /usr/bin/gcov \
    gcov /usr/bin/gcov-9

RUN apt install autoconf automake libtool curl make g++ unzip -y
RUN git clone https://github.com/google/protobuf.git
RUN cd protobuf
RUN git checkout 3.12.x
RUN git submodule update --init --recursive
RUN ./autogen.sh
RUN ./configure
RUN make
RUN make install
RUN ldconfig

WORKDIR /app
COPY . /app

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN make -j3


FROM ubuntu:rolling
LABEL description="Running container - ShipRouter"

# RUN apt update && apt -y install \
#     cmake gcc-8 g++-8 libssl-dev \
#     make binutils build-essential git \
#     zlib1g

# RUN apt-get -y --purge remove libboost-all-dev libboost-doc libboost-dev
# RUN apt-get -y install build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev
# RUN wget http://downloads.sourceforge.net/project/boost/boost/1.65.0/boost_1_65_0.tar.gz
# RUN tar -zxvf boost_1_65_0.tar.gz
# RUN cd boost_1_65_0
# RUN ./b2 --with=all -j 6 install

WORKDIR /ShipRouter

COPY --from=build /app/ShipRouter ./

CMD ["./ShipRouter"]
