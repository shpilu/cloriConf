#
# cloriConf -- a flexisible config service and service found 
# version: 1.0 
# Copyright 2018 James Wei (weijianlhp@163.com)
#

TARGET_SO=libcconf.so
TARGET_BIN=conf_test

CC=g++

TEST_SRC=$(wildcard test/*.cc)
TEST_OBJECTS=$(TEST_SRC:%.cc=%.o)
SOURCES=$(wildcard *.cc loader/zookeeper/*.cc loader/local/*.cc loader/direct/*.cc) 
OBJECTS=$(SOURCES:%.cc=%.o)

CXXFLAGS=-std=c++11 -g2 -Wall -Werror -fPIC -fno-omit-frame-pointer
LDFLAGS=
INCLUDES=-I. 

LOCAL_LD=

STATICS=
LDANYMICS=-Wl,-Bdynamic -lzookeeper_mt -lpthread -ldl $(LOCAL_LD) 

all: $(PB_SOURCES) $(TARGET_BIN) $(TARGET_SO)
	mv $(TARGET_BIN) ../bin/  

$(TARGET_BIN):$(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(STATICS) $(LDSTATICS) $(LDANYMICS)

$(TARGET_SO):$(OBJECTS)
	$(CC) $(CXXFLAGS) $^ -o $@ -shared $(LDFLAGS) $(STATICS) $(LDSTATICS) $(LDANYMICS)

$(TEST_OBJECTS):%.o:%.cc
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(OBJECTS):%.o:%.cc
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

install:

clean:
	-rm -f $(TEST_OBJECTS) $(OBJECTS) $(PB_SOURCES) $(PB_HEADERS) $(TARGET_SO) #../bin/$(TARGET_BIN)

.PHONY: all install clean
