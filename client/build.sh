#!/bin/bash

export JAVA_HOME=/usr/lib64/jdk1.8.0_25
export JAVA_INCLUDE=$JAVA_HOME/include
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

rm -f app_client
rm -f app_client.o

gcc -o app_client  app_client.c -lm -lalocs -lrados  -ljvm -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -L$LD_LIBRARY_PATH
