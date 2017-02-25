#!/bin/bash

export JAVA_HOME=/usr/lib64/jdk1.8.0_25
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

$PWD/app_client
