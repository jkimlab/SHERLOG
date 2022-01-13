FROM ubuntu:18.04

LABEL description="SHERLOG"

RUN apt-get update && apt-get install -y g++ build-essential

RUN mkdir -p /SHERLOG
COPY *.cpp /SHERLOG/
COPY *.h /SHERLOG/
COPY Makefile /SHERLOG

RUN cd /SHERLOG
RUN make -C /SHERLOG

ENV PATH /SHERLOG:$PATH
RUN sherlog 

RUN mkdir -p /work_dir
WORKDIR /work_dir
