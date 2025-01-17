/*
  Author: lyon 
  Tencent QQ: 645275593 
*/

#ifndef _dataArgs__H
#define _dataArgs__H
#include "dataArg.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"

typedef Link Args;

/* operation */
void args_deinit(Args* self);
void args_init(Args* self, Args* args);
int32_t args_getSize(Args* self);
LinkNode* args_getNode(Args* self, char* name);

Arg* args_getArgByIndex(Args* self, int32_t index);
Arg* args_getArg(Args* self, char* name);
int32_t args_removeArg(Args* self, char* name);

int32_t args_setArg(Args* self, Arg* arg);

int32_t args_copyArgByName(Args* self, char* name, Args* directList);
int32_t args_copyArg(Args* self, Arg* argToBeCopy);

char* args_getType(Args* self, char* name);
int32_t args_isArgExist(Args* self, char* name);

int32_t args_setStr(Args* self, char* name, char* strIn);
int32_t args_setStrWithDefaultName(Args* self, char* strIn);
char* args_getStr(Args* self, char* name);

int32_t args_setFloatWithDefaultName(Args* self, float argFloat);
int32_t args_setFloat(Args* self, char* name, float argFloat);
float args_getFloat(Args* self, char* name);

int32_t args_setPtr(Args* self, char* name, void* argPointer);
void* args_getPtr(Args* self, char* name);

int32_t args_setInt(Args* self, char* name, int64_t int64In);
int64_t args_getInt(Args* self, char* name);

void args_bindInt(Args* self, char* name, int32_t* intPtr);
void args_bindFloat(Args* self, char* name, float* floatPtr);
void args_bindStr(Args* self, char* name, char** stringPtr);
/* arg general opeartion */
void args_bind(Args* self, char* type, char* name, void* pointer);
char* args_print(Args* self, char* name);

int32_t args_set(Args* self, char* name, char* valueStr);
int32_t args_setObjectWithClass(Args* self,
                                char* objectName,
                                char* className,
                                void* objectPtr);
int32_t args_setPtrWithType(Args* self,
                            char* objName,
                            char* type,
                            void* objPtr);
int32_t args_foreach(Args* self,
                     int32_t (*eachHandle)(Arg* argEach, Args* handleArgs),
                     Args* handleArgs);

char* args_getBuff(Args* self, int32_t size);
uint8_t args_setLiteral(Args* self, char* targetArgName, char* literal);

Args* New_args(Args* args);
#endif
