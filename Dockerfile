FROM ubuntu:16.04

# Install some dependencies
RUN apt-get update && apt-get install -y build-essential cmake git sudo wget

# Install llvm 3.9 (based on http://apt.llvm.org/)
RUN echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main" >> /etc/apt/sources.list
RUN echo "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main"  >> /etc/apt/sources.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15CF4D18AF4F7421
RUN apt-get update && apt-get install -y clang-3.9 clang-3.9-doc libclang-common-3.9-dev libclang-3.9-dev libclang1-3.9 libclang1-3.9-dbg libllvm-3.9-ocaml-dev libllvm3.9 libllvm3.9-dbg lldb-3.9 llvm-3.9 llvm-3.9-dev llvm-3.9-doc llvm-3.9-examples llvm-3.9-runtime clang-format-3.9 python-clang-3.9 libfuzzer-3.9-dev


# Add a non-root user
RUN useradd --create-home --shell /bin/bash sip && adduser sip sudo
RUN echo 'sip:sip' | chpasswd
WORKDIR /home/sip

#
# WORKDIR "/home/sip"
RUN git clone https://github.com/stone-z/sip-oblivious-hashing.git
#

# Copy pass files
# COPY files/home/ /home/sip/
COPY files/home/ /home/sip/sip-oblivious-hashing
COPY files/include/ /usr/local/include/
COPY files/lib/ /usr/local/lib/

# Nano for convenience
RUN apt-get install nano

# Switch to user
RUN chown -R sip:sip .
USER sip

# Build and run Skeleton pass example
WORKDIR /home/sip/sip-oblivious-hashing/input-dependency
RUN mkdir build
WORKDIR  /home/sip/sip-oblivious-hashing/input-dependency/build
RUN cmake ..
RUN make
WORKDIR  /home/sip/sip-oblivious-hashing
#

# Build OH Transformation Pass
WORKDIR /home/sip/sip-oblivious-hashing/oblivious-hashing-transformation
RUN mkdir build
WORKDIR  /home/sip/sip-oblivious-hashing/oblivious-hashing-transformation/build
RUN cmake ..
RUN make
WORKDIR  /home/sip/sip-oblivious-hashing
#

# Build OH inject Pass
WORKDIR /home/sip/sip-oblivious-hashing/oblivious-hashing-inject
RUN mkdir build
WORKDIR  /home/sip/sip-oblivious-hashing/oblivious-hashing-inject/build
RUN cmake ..
RUN make
WORKDIR  /home/sip/sip-oblivious-hashing/oblivious-hashing-inject
WORKDIR  /home/sip/sip-oblivious-hashing
#

