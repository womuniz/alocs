#!/bin/bash

export JAVA_HOME=/usr/lib/jvm/java-8-oracle
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

export JAR_PATH=mds/jar
export CLASSPATH=$JAR_PATH/zookeeper-3.4.6.jar:$JAR_PATH/junit-4.11.jar:$JAR_PATH/log4j-1.2.16.jar
export CLASSPATH=$CLASSPATH:$JAR_PATH/slf4j-api-1.6.3.jar:$JAR_PATH/slf4j-log4j12-1.7.0.jar

export JAVA_INCLUDE=$JAVA_HOME/include

rm -f mds/*.class
rm -f *.o
rm -f *.so
sudo rm -f /usr/lib/libalocs.so

javac -sourcepath mds mds/Auxiliary.java mds/IntervalTree.java mds/ClusterMappingEstructure.java mds/InterfaceJava.java

gcc -c -Werror -fpic src/imds.c -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -ljvm src/util.c src/cache.c src/common.c src/ceph_lib.c src/iceph.c src/libalocs.c
gcc -shared -o libalocs.so common.o libalocs.o iceph.o ceph_lib.o cache.o util.o imds.o

sudo cp *.so /usr/lib/.
sudo cp src/include/*.h /usr/include/alocs/.
