/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef _PikaObj_baseObj__H
#define _PikaObj_baseObj__H
#include "PikaObj.h"
#include "TinyObj.h"
#include "dataMemory.h"

PikaObj* New_BaseObj(Args* args);
int32_t obj_newObj(PikaObj* self, char* objPath, char* classPath);
int32_t obj_import(PikaObj* self, char* className, NewFun classPtr);
int32_t obj_newObjByFun(PikaObj* self,
                        char* objName,
                        char* className,
                        void* newFunPtr);
#endif
