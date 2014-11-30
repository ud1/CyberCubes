/*
** Lua binding: Block
** Generated automatically by tolua++-1.0.93 on Sat Oct 25 11:33:23 2014.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Block_open (lua_State* tolua_S);

#include "Block.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_Block (lua_State* tolua_S)
{
 Block* self = (Block*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Block");
}

/* method: new of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_new00
static int tolua_Block_Block_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Block* tolua_ret = (Block*)  Mtolua_new((Block)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Block");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_new00_local
static int tolua_Block_Block_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Block* tolua_ret = (Block*)  Mtolua_new((Block)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Block");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: id of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_id
static int tolua_get_Block_id(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'id'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->id);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: id of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_id
static int tolua_set_Block_id(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'id'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->id = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: name of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_name
static int tolua_get_Block_name(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'name'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->name);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: name of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_name
static int tolua_set_Block_name(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'name'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->name = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: mainTexture of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_mainTexture
static int tolua_get_Block_mainTexture(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'mainTexture'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->mainTexture);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: mainTexture of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_mainTexture
static int tolua_set_Block_mainTexture(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'mainTexture'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->mainTexture = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: opaque of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_opaque
static int tolua_get_Block_opaque(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opaque'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->opaque);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: opaque of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_opaque
static int tolua_set_Block_opaque(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opaque'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->opaque = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: lightOpacity of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightOpacity
static int tolua_get_Block_lightOpacity(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightOpacity'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightOpacity);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightOpacity of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightOpacity
static int tolua_set_Block_lightOpacity(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightOpacity'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightOpacity = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: lightValue of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightValue
static int tolua_get_Block_lightValue(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValue'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightValue of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightValue
static int tolua_set_Block_lightValue(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValue'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightValue = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: hardness of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_hardness
static int tolua_get_Block_hardness(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'hardness'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->hardness);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: hardness of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_hardness
static int tolua_set_Block_hardness(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'hardness'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->hardness = ((float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: registerBlock of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_registerBlock00
static int tolua_Block_Block_registerBlock00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'registerBlock'", NULL);
#endif
  {
   self->registerBlock();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'registerBlock'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_get00
static int tolua_Block_Block_get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   const Block* tolua_ret = (const Block*)  Block::get(id);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"const Block");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_get01
static int tolua_Block_Block_get01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   const Block* tolua_ret = (const Block*)  Block::get(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"const Block");
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 2;
tolua_lerror:
 return tolua_Block_Block_get00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Block_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"Block","Block","",tolua_collect_Block);
  #else
  tolua_cclass(tolua_S,"Block","Block","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"Block");
   tolua_function(tolua_S,"new",tolua_Block_Block_new00);
   tolua_function(tolua_S,"new_local",tolua_Block_Block_new00_local);
   tolua_function(tolua_S,".call",tolua_Block_Block_new00_local);
   tolua_variable(tolua_S,"id",tolua_get_Block_id,tolua_set_Block_id);
   tolua_variable(tolua_S,"name",tolua_get_Block_name,tolua_set_Block_name);
   tolua_variable(tolua_S,"mainTexture",tolua_get_Block_mainTexture,tolua_set_Block_mainTexture);
   tolua_variable(tolua_S,"opaque",tolua_get_Block_opaque,tolua_set_Block_opaque);
   tolua_variable(tolua_S,"lightOpacity",tolua_get_Block_lightOpacity,tolua_set_Block_lightOpacity);
   tolua_variable(tolua_S,"lightValue",tolua_get_Block_lightValue,tolua_set_Block_lightValue);
   tolua_variable(tolua_S,"hardness",tolua_get_Block_hardness,tolua_set_Block_hardness);
   tolua_function(tolua_S,"registerBlock",tolua_Block_Block_registerBlock00);
   tolua_function(tolua_S,"get",tolua_Block_Block_get00);
   tolua_function(tolua_S,"get",tolua_Block_Block_get01);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Block (lua_State* tolua_S) {
 return tolua_Block_open(tolua_S);
};
#endif

