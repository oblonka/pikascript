#include "MimiObj.h"
#include "TinyObj.h"
#include "method.h"
#include "SysObj.h"
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static void newObjMethod(MimiObj *self, Args *args)
{
    /* get arg */
    char *objPath = args_getStr(args, "objPath");
    char *classPath = args_getStr(args, "classPath");
    int32_t res = obj_newObj(self, objPath, classPath);
    if (1 == res)
    {
        method_sysOut(args, "[error] new: class not found .");
        method_setErrorCode(args, 1);
        return;
    }
}

static void type(MimiObj *obj, Args *args)
{
    args_setInt(args, "errCode", 0);
    char *argPath = args_getStr(args, "argPath");
    if (NULL == argPath)
    {
        /* no input obj path, use current obj */
        MimiObj *objHost = obj_getPtr(obj, "_ctx");
        Arg *objArg = obj_getArg(objHost, obj->name);
        if (NULL == objArg)
        {
            method_sysOut(args, "[error] type: arg no found.");
            method_setErrorCode(args, 1);
            return;
        }
        method_sysOut(args, arg_getType(objArg));
        return;
    }
    Arg *arg = obj_getArg(obj, argPath);
    if (NULL == arg)
    {
        method_sysOut(args, "[error] type: arg no found.");
        method_setErrorCode(args, 1);
        return;
    }
    method_sysOut(args, arg_getType(arg));
}

static void del(MimiObj *obj, Args *args)
{
    args_setInt(args, "errCode", 0);
    char *argPath = args_getStr(args, "argPath");
    int32_t res = obj_removeArg(obj, argPath);
    if (1 == res)
    {
        method_sysOut(args, "[error] del: object no found.");
        args_setInt(args, "errCode", 1);
        return;
    }
    if (2 == res)
    {
        method_sysOut(args, "[error] del: arg not match.");
        args_setInt(args, "errCode", 2);
        return;
    }
}

static void set(MimiObj *obj, Args *args)
{
    args_setInt(args, "errCode", 0);
    char *argPath = method_getStr(args, "argPath");
    if (obj_isArgExist(obj, argPath))
    {
        /* update arg */
        char *valStr = args_print(args, "val");
        int32_t res = obj_set(obj, argPath, valStr);
        if (1 == res)
        {
            method_sysOut(args, "[error] set: arg no found.");
            args_setInt(args, "errCode", 1);
            return;
        }
        if (2 == res)
        {
            method_sysOut(args, "[error] set: type not match.");
            args_setInt(args, "errCode", 1);
            return;
        }
        if (3 == res)
        {
            method_sysOut(args, "[error] set: object not found.");
            args_setInt(args, "errCode", 1);
            return;
        }
        return;
    }
    /* new arg */
    Arg *val = args_getArg(args, "val");
    Arg *newArg = arg_copy(val);
    char *argName = strsGetLastToken(args, argPath, '.');
    arg_setName(newArg, argName);
    int32_t res = obj_setArg(obj, argPath, newArg);
    if (res == 1)
    {
        method_sysOut(args, "[error] set: object not found.");
        args_setInt(args, "errCode", 1);
    }
    arg_deinit(newArg);
    return;
}

static int32_t listEachArg(Arg *argEach, Args *handleArgs)
{
    Args *buffs = handleArgs;
    if (NULL == handleArgs)
    {
        /* error: not handleArgs input */
        return 1;
    }

    char *argName = strsCopy(buffs, arg_getName(argEach));
    if (strIsStartWith(argName, "["))
    {
        /* skip */
        return 0;
    }

    char *stringOut = args_getStr(handleArgs, "stringOut");
    if (NULL == stringOut)
    {
        // stringOut no found
        return 1;
    }

    strAppend(stringOut, argName);
    strAppend(stringOut, " ");
    args_setStr(handleArgs, "stringOut", stringOut);
    return 0;
}

static void list(MimiObj *self, Args *args)
{
    char *objPath = args_getStr(args, "objPath");
    args_setInt(args, "errCode", 0);
    args_setStr(args, "stringOut", "");
    if (NULL == objPath)
    {
        /* no input obj path, use current obj */
        args_foreach(self->attributeList, listEachArg, args);
        method_sysOut(args, args_getStr(args, "stringOut"));
        return;
    }
    MimiObj *obj = obj_getObj(self, objPath, 0);
    if (NULL == obj)
    {
        /* do not find obj */
        method_sysOut(args, "[error] list: object no found.");
        method_setErrorCode(args, 1);
        return;
    }
    /* list args */
    args_foreach(obj->attributeList, listEachArg, args);
    method_sysOut(args, args_getStr(args, "stringOut"));
    return;
}

static void print(MimiObj *obj, Args *args)
{
    args_setInt(args, "errCode", 0);
    char *res = args_print(args, "val");
    if (NULL == res)
    {
        method_sysOut(args, "[error] print: can not print32_t val");
        args_setInt(args, "errCode", 1);
        return;
    }
    /* not empty */
    method_sysOut(args, res);
}



int32_t loadExceptMethod(Arg *argEach, Args *handleArgs)
{
    char *argName = arg_getName(argEach);
    if (strIsStartWith(argName, "[md]"))
    {
        /* skip method declearation */
        // return 0;
    }
    if (strIsStartWith(argName, "[mp]"))
    {
        /* skip method pointer */
        // return 0;
    }
    args_copyArg(handleArgs, argEach);
    return 0;
}

MimiObj *obj_loadWithoutMethod(MimiObj *thisClass)
{
    MimiObj *newObj = New_TinyObj(NULL);
    Args *thisClassArgs = thisClass->attributeList;
    Args *newObjArgs = newObj->attributeList;
    args_foreach(thisClassArgs, loadExceptMethod, newObjArgs);
    return newObj;
}

MimiObj *New_SysObj(Args *args)
{
    /* derive */
    MimiObj *self = New_BaseObj(args);

    /* attribute */

    /* method */
    class_defineMethod(self, "print(val:any)", print);
    class_defineMethod(self, "set(argPath:str, val:any)", set);
    class_defineMethod(self, "ls(objPath:str)", list);
    class_defineMethod(self, "del(argPath:str)", del);
    class_defineMethod(self, "type(argPath:str)", type);
    class_defineMethod(self, "new(objPath:str,classPath:str)", newObjMethod);

    /* override */

    return self;
}
