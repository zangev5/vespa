# Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
vespa_add_library(fnet
    SOURCES
    channel.cpp
    channellookup.cpp
    config.cpp
    connection.cpp
    connector.cpp
    context.cpp
    controlpacket.cpp
    databuffer.cpp
    dummypacket.cpp
    info.cpp
    iocomponent.cpp
    packet.cpp
    packetqueue.cpp
    scheduler.cpp
    signalshutdown.cpp
    simplepacketstreamer.cpp
    stats.cpp
    task.cpp
    transport.cpp
    transport_thread.cpp
    $<TARGET_OBJECTS:fnet_frt>
    INSTALL lib64
    DEPENDS
)
