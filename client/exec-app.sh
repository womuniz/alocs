#!/bin/bash

export JAVA_HOME=/usr/lib/jvm/java-8-oracle
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

$PWD/app_client
