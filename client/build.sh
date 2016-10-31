#!/bin/bash
#[libjvm.so+0x6e0c78]  jni_GetStringUTFChars+0xb8
export JAVA_HOME=/usr/lib/jvm/java-8-oracle
export JAVA_INCLUDE=$JAVA_HOME/include
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

rm -f app_client
rm -f app_client.o

gcc -o app_client  app_client.c -lm -lalocs -lrados  -ljvm -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -L$LD_LIBRARY_PATH
