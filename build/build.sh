#!/bin/bash

export ALOCS_DIR=$HOME/Documents/academico/mestrado/implementacao/alocs
export SRC_DIR=$ALOCS_DIR/src
export MDS_DIR=$ALOCS_DIR/mds
export JAVA_HOME=/usr/lib64/jdk1.8.0_25
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server

export JAR_PATH=$MDS_DIR/jar
export CLASSPATH=$JAR_PATH/zookeeper-3.4.6.jar:$JAR_PATH/junit-4.11.jar:$JAR_PATH/log4j-1.2.16.jar
export CLASSPATH=$CLASSPATH:$JAR_PATH/slf4j-api-1.6.3.jar:$JAR_PATH/slf4j-log4j12-1.7.0.jar

export JAVA_INCLUDE=$JAVA_HOME/include

rm -f $MDS_DIR/*.class
rm -f *.o
rm -f *.so
sudo rm -f /usr/lib/libalocs.so

javac -sourcepath $MDS_DIR $MDS_DIR/Auxiliary.java $MDS_DIR/IntervalTree.java $MDS_DIR/ClusterMappingEstructure.java $MDS_DIR/InterfaceJava.java

#gcc -c -Werror -B$SRC_DIR -fpic imds.c -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -ljvm util.c hashmap.c src/libalocs.c src/control.c src/up_queue.c src/metadata.c src/storage.c src/iceph.c src/ceph_lib.c src/cache.c src/cache_heap.c src/cache_htable.c
#gcc -shared -o libalocs.so util.o hashmap.o libalocs.o control.o up_queue.o metadata.o imds.o storage.o iceph.o ceph_lib.o cache.o cache_heap.o cache_htable.o

gcc -c -Werror -L$SRC_DIR -fpic $SRC_DIR/imds.c -I$JAVA_INCLUDE -I$JAVA_INCLUDE/linux -ljvm $SRC_DIR/util.c $SRC_DIR/hashmap.c $SRC_DIR/libalocs.c $SRC_DIR/control.c $SRC_DIR/up_queue.c $SRC_DIR/metadata.c $SRC_DIR/storage.c $SRC_DIR/iceph.c $SRC_DIR/ceph_lib.c $SRC_DIR/cache.c $SRC_DIR/cache_heap.c $SRC_DIR/cache_htable.c
gcc -shared -o libalocs.so util.o hashmap.o libalocs.o control.o up_queue.o metadata.o imds.o storage.o iceph.o ceph_lib.o cache.o cache_heap.o cache_htable.o

sudo cp *.so /usr/lib/.
sudo cp $SRC_DIR/include/*.h /usr/include/alocs/.
