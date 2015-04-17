#ifndef TYPES_H
#define TYPES_H

enum Type {TYPE_NOT_DEFINED = 37, TYPE_QSTRING, TYPE_STATION, TYPE_SECTION, TYPE_PVR, TYPE_REQUEST, TYPE_STREAM, TYPE_XML_F2};
enum Commands {PLAN_BUZ = 37, PLAN_SUZ, CHANGE_TO, CACHE_OUT, LOAD_REQUEST_DIKON, LOAD_REQUEST_ZHENYA, ACCEPT_OFFSET, GET_F2};
//should wait for respond to offset Stream from Client or just do it
#define WAIT_FOR_RESPOND
//prevents Streams to be planned without offset (graph.cpp)
//#define TEST_MOVE_STREAM
//local testing (need to fix after Client<-->Server implementation) (testplan.cpp)
//#define TEST

#endif // TYPES_H
