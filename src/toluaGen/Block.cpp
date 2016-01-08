/*
** Lua binding: Block
** Generated automatically by tolua++-1.0.93 on Fri Jan  8 19:54:20 2016.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Block_open (lua_State* tolua_S);

#include "blockType.hpp"
#include "Block.hpp"

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
 tolua_usertype(tolua_S,"std::unordered_map<int,Block>");
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

/* get function: textureXN of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureXN
static int tolua_get_Block_textureXN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureXN'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureXN);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureXN of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureXN
static int tolua_set_Block_textureXN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureXN'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureXN = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: textureXP of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureXP
static int tolua_get_Block_textureXP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureXP'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureXP);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureXP of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureXP
static int tolua_set_Block_textureXP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureXP'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureXP = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: textureYN of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureYN
static int tolua_get_Block_textureYN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureYN'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureYN);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureYN of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureYN
static int tolua_set_Block_textureYN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureYN'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureYN = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: textureYP of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureYP
static int tolua_get_Block_textureYP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureYP'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureYP);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureYP of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureYP
static int tolua_set_Block_textureYP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureYP'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureYP = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: textureZN of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureZN
static int tolua_get_Block_textureZN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureZN'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureZN);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureZN of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureZN
static int tolua_set_Block_textureZN(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureZN'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureZN = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: textureZP of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_textureZP
static int tolua_get_Block_textureZP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureZP'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->textureZP);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureZP of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_textureZP
static int tolua_set_Block_textureZP(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'textureZP'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->textureZP = ((std::string)  tolua_tocppstring(tolua_S,2,0))
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

/* get function: lightValueSun of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightValueSun
static int tolua_get_Block_lightValueSun(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueSun'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightValueSun);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightValueSun of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightValueSun
static int tolua_set_Block_lightValueSun(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueSun'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightValueSun = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: lightValueR of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightValueR
static int tolua_get_Block_lightValueR(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueR'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightValueR);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightValueR of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightValueR
static int tolua_set_Block_lightValueR(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueR'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightValueR = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: lightValueG of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightValueG
static int tolua_get_Block_lightValueG(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueG'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightValueG);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightValueG of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightValueG
static int tolua_set_Block_lightValueG(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueG'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightValueG = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: lightValueB of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_lightValueB
static int tolua_get_Block_lightValueB(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueB'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->lightValueB);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: lightValueB of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_lightValueB
static int tolua_set_Block_lightValueB(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'lightValueB'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->lightValueB = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rotationGroup of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_rotationGroup
static int tolua_get_Block_rotationGroup(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rotationGroup'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->rotationGroup);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rotationGroup of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_rotationGroup
static int tolua_set_Block_rotationGroup(lua_State* tolua_S)
{
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rotationGroup'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rotationGroup = ((int)  tolua_tonumber(tolua_S,2,0))
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

/* get function: textureId of class  Block */
#ifndef TOLUA_DISABLE_tolua_get_Block_Block_textureId
static int tolua_get_Block_Block_textureId(lua_State* tolua_S)
{
 int tolua_index;
  Block* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (Block*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=6)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->textureId[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: textureId of class  Block */
#ifndef TOLUA_DISABLE_tolua_set_Block_Block_textureId
static int tolua_set_Block_Block_textureId(lua_State* tolua_S)
{
 int tolua_index;
  Block* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (Block*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=6)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->textureId[tolua_index] = ((int)  tolua_tonumber(tolua_S,3,0));
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

/* method: getTexture of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_getTexture00
static int tolua_Block_Block_getTexture00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Block",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Block* self = (Block*)  tolua_tousertype(tolua_S,1,0);
  int side = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTexture'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->getTexture(side);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTexture'.",&tolua_err);
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
   Block* tolua_ret = (Block*)  Block::get(id);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Block");
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

/* method: getBlockIdMap of class  Block */
#ifndef TOLUA_DISABLE_tolua_Block_Block_getBlockIdMap00
static int tolua_Block_Block_getBlockIdMap00(lua_State* tolua_S)
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
   const std::unordered_map<int,Block>& tolua_ret = (const std::unordered_map<int,Block>&)  Block::getBlockIdMap();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const std::unordered_map<int,Block>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getBlockIdMap'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Block_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"BLOCK_AIR",BLOCK_AIR);
  tolua_constant(tolua_S,"BLOCK_STONE",BLOCK_STONE);
  tolua_constant(tolua_S,"BLOCK_DIRT",BLOCK_DIRT);
  tolua_constant(tolua_S,"BLOCK_COBBLESTONE",BLOCK_COBBLESTONE);
  tolua_constant(tolua_S,"BLOCK_BRICK",BLOCK_BRICK);
  tolua_constant(tolua_S,"BLOCK_GRAVEL",BLOCK_GRAVEL);
  tolua_constant(tolua_S,"BLOCK_STONEBRICK",BLOCK_STONEBRICK);
  tolua_constant(tolua_S,"BLOCK_STONE_SLAB_FULL",BLOCK_STONE_SLAB_FULL);
  tolua_constant(tolua_S,"BLOCK_LIGHT_BLOCK",BLOCK_LIGHT_BLOCK);
  tolua_constant(tolua_S,"BLOCK_PLANKS",BLOCK_PLANKS);
  tolua_constant(tolua_S,"BLOCK_BEDROCK",BLOCK_BEDROCK);
  tolua_constant(tolua_S,"BLOCK_SAND",BLOCK_SAND);
  tolua_constant(tolua_S,"BLOCK_GOLD_ORE",BLOCK_GOLD_ORE);
  tolua_constant(tolua_S,"BLOCK_IRON_ORE",BLOCK_IRON_ORE);
  tolua_constant(tolua_S,"BLOCK_COAL_ORE",BLOCK_COAL_ORE);
  tolua_constant(tolua_S,"BLOCK_LOG",BLOCK_LOG);
  tolua_constant(tolua_S,"BLOCK_SPONGE",BLOCK_SPONGE);
  tolua_constant(tolua_S,"BLOCK_LAPIS_ORE",BLOCK_LAPIS_ORE);
  tolua_constant(tolua_S,"BLOCK_DISPENSER",BLOCK_DISPENSER);
  tolua_constant(tolua_S,"BLOCK_SANDSTONE",BLOCK_SANDSTONE);
  tolua_constant(tolua_S,"BLOCK_WOOL",BLOCK_WOOL);
  tolua_constant(tolua_S,"BLOCK_GOLD_BLOCK",BLOCK_GOLD_BLOCK);
  tolua_constant(tolua_S,"BLOCK_TNT",BLOCK_TNT);
  tolua_constant(tolua_S,"BLOCK_BOOKSHELF",BLOCK_BOOKSHELF);
  tolua_constant(tolua_S,"BLOCK_MOSSY_COBBLESTONE",BLOCK_MOSSY_COBBLESTONE);
  tolua_constant(tolua_S,"BLOCK_OBSIDIAN",BLOCK_OBSIDIAN);
  tolua_constant(tolua_S,"BLOCK_DIAMOND_ORE",BLOCK_DIAMOND_ORE);
  tolua_constant(tolua_S,"BLOCK_CRAFTING_TABLE",BLOCK_CRAFTING_TABLE);
  tolua_constant(tolua_S,"BLOCK_FARMLAND",BLOCK_FARMLAND);
  tolua_constant(tolua_S,"BLOCK_FURNACE",BLOCK_FURNACE);
  tolua_constant(tolua_S,"BLOCK_REDSTONE_ORE",BLOCK_REDSTONE_ORE);
  tolua_constant(tolua_S,"BLOCK_ICE",BLOCK_ICE);
  tolua_constant(tolua_S,"BLOCK_SNOW",BLOCK_SNOW);
  tolua_constant(tolua_S,"BLOCK_CLAY",BLOCK_CLAY);
  tolua_constant(tolua_S,"BLOCK_PUMPKIN",BLOCK_PUMPKIN);
  tolua_constant(tolua_S,"BLOCK_NETHERRACK",BLOCK_NETHERRACK);
  tolua_constant(tolua_S,"BLOCK_SOUL_SAND",BLOCK_SOUL_SAND);
  tolua_constant(tolua_S,"BLOCK_GLOWSTONE",BLOCK_GLOWSTONE);
  tolua_constant(tolua_S,"BLOCK_BROWN_MUSHROOM_BLOCK",BLOCK_BROWN_MUSHROOM_BLOCK);
  tolua_constant(tolua_S,"BLOCK_RED_MUSHROOM_BLOCK",BLOCK_RED_MUSHROOM_BLOCK);
  tolua_constant(tolua_S,"BLOCK_MELON_BLOCK",BLOCK_MELON_BLOCK);
  tolua_constant(tolua_S,"BLOCK_MYCELIUM",BLOCK_MYCELIUM);
  tolua_constant(tolua_S,"BLOCK_NETHER_BRICK",BLOCK_NETHER_BRICK);
  tolua_constant(tolua_S,"BLOCK_END_STONE",BLOCK_END_STONE);
  tolua_constant(tolua_S,"BLOCK_EMERALD_ORE",BLOCK_EMERALD_ORE);
  tolua_constant(tolua_S,"BLOCK_QUARTZ_ORE",BLOCK_QUARTZ_ORE);
  tolua_constant(tolua_S,"BLOCK_STAINED_HARDENED_CLAY",BLOCK_STAINED_HARDENED_CLAY);
  tolua_constant(tolua_S,"BLOCK_LOG2",BLOCK_LOG2);
  tolua_constant(tolua_S,"BLOCK_PRISMARINE",BLOCK_PRISMARINE);
  tolua_constant(tolua_S,"BLOCK_SEA_LANTERN",BLOCK_SEA_LANTERN);
  tolua_constant(tolua_S,"BLOCK_HARDENED_CLAY",BLOCK_HARDENED_CLAY);
  tolua_constant(tolua_S,"BLOCK_PACKED_ICE",BLOCK_PACKED_ICE);
  tolua_constant(tolua_S,"BLOCK_GLASS",BLOCK_GLASS);
  tolua_constant(tolua_S,"BLOCK_LEAVES",BLOCK_LEAVES);
  tolua_constant(tolua_S,"BLOCK_GRASS",BLOCK_GRASS);
  tolua_constant(tolua_S,"BLOCK_TORCH",BLOCK_TORCH);
  tolua_constant(tolua_S,"BLOCK_FIRE",BLOCK_FIRE);
  tolua_constant(tolua_S,"BLOCK_WHEAT",BLOCK_WHEAT);
  tolua_constant(tolua_S,"BLOCK_LADDER",BLOCK_LADDER);
  tolua_constant(tolua_S,"BLOCK_LEVER",BLOCK_LEVER);
  tolua_constant(tolua_S,"BLOCK_STONE_BUTTON",BLOCK_STONE_BUTTON);
  tolua_constant(tolua_S,"BLOCK_REEDS",BLOCK_REEDS);
  tolua_constant(tolua_S,"BLOCK_GLASS_PANE",BLOCK_GLASS_PANE);
  tolua_constant(tolua_S,"BLOCK_FLOWING_WATER",BLOCK_FLOWING_WATER);
  tolua_constant(tolua_S,"BLOCK_FLOWING_LAVA",BLOCK_FLOWING_LAVA);
  tolua_constant(tolua_S,"BLOCK_LAVA",BLOCK_LAVA);
  tolua_constant(tolua_S,"BLOCK_WEB",BLOCK_WEB);
  tolua_constant(tolua_S,"BLOCK_TALLGRASS",BLOCK_TALLGRASS);
  tolua_constant(tolua_S,"BLOCK_DEADBUSH",BLOCK_DEADBUSH);
  tolua_constant(tolua_S,"BLOCK_YELLOW_FLOWER",BLOCK_YELLOW_FLOWER);
  tolua_constant(tolua_S,"BLOCK_RED_FLOWER",BLOCK_RED_FLOWER);
  tolua_constant(tolua_S,"BLOCK_BROWN_MUSHROOM",BLOCK_BROWN_MUSHROOM);
  tolua_constant(tolua_S,"BLOCK_RED_MUSHROOM",BLOCK_RED_MUSHROOM);
  tolua_constant(tolua_S,"BLOCK_STONE_SLAB",BLOCK_STONE_SLAB);
  tolua_constant(tolua_S,"BLOCK_MOB_SPAWNER",BLOCK_MOB_SPAWNER);
  tolua_constant(tolua_S,"BLOCK_OAK_STAIRS",BLOCK_OAK_STAIRS);
  tolua_constant(tolua_S,"BLOCK_CHEST",BLOCK_CHEST);
  tolua_constant(tolua_S,"BLOCK_REDSTONE_WIRE",BLOCK_REDSTONE_WIRE);
  tolua_constant(tolua_S,"BLOCK_WOODEN_DOOR",BLOCK_WOODEN_DOOR);
  tolua_constant(tolua_S,"BLOCK_RAIL",BLOCK_RAIL);
  tolua_constant(tolua_S,"BLOCK_STONE_STAIRS",BLOCK_STONE_STAIRS);
  tolua_constant(tolua_S,"BLOCK_STONE_PRESSURE_PLATE",BLOCK_STONE_PRESSURE_PLATE);
  tolua_constant(tolua_S,"BLOCK_IRON_DOOR",BLOCK_IRON_DOOR);
  tolua_constant(tolua_S,"BLOCK_WOODEN_PRESSURE_PLATE",BLOCK_WOODEN_PRESSURE_PLATE);
  tolua_constant(tolua_S,"BLOCK_SNOW_LAYER",BLOCK_SNOW_LAYER);
  tolua_constant(tolua_S,"BLOCK_CACTUS",BLOCK_CACTUS);
  tolua_constant(tolua_S,"BLOCK_FENCE",BLOCK_FENCE);
  tolua_constant(tolua_S,"BLOCK_PORTAL",BLOCK_PORTAL);
  tolua_constant(tolua_S,"BLOCK_UNPOWERED_REPEATER",BLOCK_UNPOWERED_REPEATER);
  tolua_constant(tolua_S,"BLOCK_MONSTER_EGG",BLOCK_MONSTER_EGG);
  tolua_constant(tolua_S,"BLOCK_IRON_BARS",BLOCK_IRON_BARS);
  tolua_constant(tolua_S,"BLOCK_VINE",BLOCK_VINE);
  tolua_constant(tolua_S,"BLOCK_WATERLILY",BLOCK_WATERLILY);
  tolua_constant(tolua_S,"BLOCK_NETHER_BRICK_FENCE",BLOCK_NETHER_BRICK_FENCE);
  tolua_constant(tolua_S,"BLOCK_NETHER_BRICK_STAIRS",BLOCK_NETHER_BRICK_STAIRS);
  tolua_constant(tolua_S,"BLOCK_NETHER_WART",BLOCK_NETHER_WART);
  tolua_constant(tolua_S,"BLOCK_CAULDRON",BLOCK_CAULDRON);
  tolua_constant(tolua_S,"BLOCK_END_PORTAL",BLOCK_END_PORTAL);
  tolua_constant(tolua_S,"BLOCK_END_PORTAL_FRAME",BLOCK_END_PORTAL_FRAME);
  tolua_constant(tolua_S,"BLOCK_DRAGON_EGG",BLOCK_DRAGON_EGG);
  tolua_constant(tolua_S,"BLOCK_COCOA",BLOCK_COCOA);
  tolua_constant(tolua_S,"BLOCK_SANDSTONE_STAIRS",BLOCK_SANDSTONE_STAIRS);
  tolua_constant(tolua_S,"BLOCK_TRIPWIRE_HOOK",BLOCK_TRIPWIRE_HOOK);
  tolua_constant(tolua_S,"BLOCK_TRIPWIRE",BLOCK_TRIPWIRE);
  tolua_constant(tolua_S,"BLOCK_FLOWER_POT",BLOCK_FLOWER_POT);
  tolua_constant(tolua_S,"BLOCK_CARROTS",BLOCK_CARROTS);
  tolua_constant(tolua_S,"BLOCK_POTATOES",BLOCK_POTATOES);
  tolua_constant(tolua_S,"BLOCK_LEAVES2",BLOCK_LEAVES2);
  tolua_constant(tolua_S,"BLOCK_DOUBLE_PLANT",BLOCK_DOUBLE_PLANT);
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
   tolua_variable(tolua_S,"textureXN",tolua_get_Block_textureXN,tolua_set_Block_textureXN);
   tolua_variable(tolua_S,"textureXP",tolua_get_Block_textureXP,tolua_set_Block_textureXP);
   tolua_variable(tolua_S,"textureYN",tolua_get_Block_textureYN,tolua_set_Block_textureYN);
   tolua_variable(tolua_S,"textureYP",tolua_get_Block_textureYP,tolua_set_Block_textureYP);
   tolua_variable(tolua_S,"textureZN",tolua_get_Block_textureZN,tolua_set_Block_textureZN);
   tolua_variable(tolua_S,"textureZP",tolua_get_Block_textureZP,tolua_set_Block_textureZP);
   tolua_variable(tolua_S,"opaque",tolua_get_Block_opaque,tolua_set_Block_opaque);
   tolua_variable(tolua_S,"lightOpacity",tolua_get_Block_lightOpacity,tolua_set_Block_lightOpacity);
   tolua_variable(tolua_S,"lightValueSun",tolua_get_Block_lightValueSun,tolua_set_Block_lightValueSun);
   tolua_variable(tolua_S,"lightValueR",tolua_get_Block_lightValueR,tolua_set_Block_lightValueR);
   tolua_variable(tolua_S,"lightValueG",tolua_get_Block_lightValueG,tolua_set_Block_lightValueG);
   tolua_variable(tolua_S,"lightValueB",tolua_get_Block_lightValueB,tolua_set_Block_lightValueB);
   tolua_variable(tolua_S,"rotationGroup",tolua_get_Block_rotationGroup,tolua_set_Block_rotationGroup);
   tolua_variable(tolua_S,"hardness",tolua_get_Block_hardness,tolua_set_Block_hardness);
   tolua_array(tolua_S,"textureId",tolua_get_Block_Block_textureId,tolua_set_Block_Block_textureId);
   tolua_function(tolua_S,"registerBlock",tolua_Block_Block_registerBlock00);
   tolua_function(tolua_S,"getTexture",tolua_Block_Block_getTexture00);
   tolua_function(tolua_S,"get",tolua_Block_Block_get00);
   tolua_function(tolua_S,"get",tolua_Block_Block_get01);
   tolua_function(tolua_S,"getBlockIdMap",tolua_Block_Block_getBlockIdMap00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Block (lua_State* tolua_S) {
 return tolua_Block_open(tolua_S);
};
#endif

