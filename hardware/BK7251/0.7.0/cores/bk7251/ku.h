#ifndef __KU_H
#define __KU_H
#include <Arduino.h>

extern int _b1111;
class B{
public:
  B(){
	  rt_kprintf("B()+++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
  }
  B(int b){
	  _b1111 = b;
	  rt_kprintf("B __b1111=%d, _b=%d----------------------------\r\n",_b1111,_b);
  }
  ~B(){
	  rt_kprintf("~B***************************************************\r\n");
	  
  }
  void begin(){
	  
	  rt_kprintf("begin _b1111=%d 0x%x,_b=%d 0x%x\r\n",_b1111,&_b1111,_b, &_b);
  }
   
private:
  int _b;
};


//extern B g;
#endif