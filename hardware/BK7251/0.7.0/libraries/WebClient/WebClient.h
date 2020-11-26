#ifndef __WEBCLIENT_H
#define __WEBCLIENT_H

extern "C"{
#include "webclient1.h"
}


class WebClient{
public:
  WebClient(IPAddress addr, int port = 80);
  virtual ~WebServer();
};

#endif