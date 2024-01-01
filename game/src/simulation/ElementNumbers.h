// 3293562998
// Do not modify the hash above, it is used for determining
// if the element list has changed

// Ensure you are modifying the .template version, the
// .h file is auto-generated and will be overwritten
// when the elements table is changed. To add/remove
// elements, edit the premake5.lua in simulation/elements 

#ifdef ELEMENT_NUMBERS_CALL
# define ELEMENT_DEFINE(name, id) elements[id].Element_ ## name ()
#endif
#ifdef ELEMENT_NUMBERS_DECLARE
# define ELEMENT_DEFINE(name, id) void Element_ ## name ()
#endif
#ifdef ELEMENT_NUMBERS_ENUMERATE
# define ELEMENT_DEFINE(name, id) constexpr int PT_ ## name = id
#endif

ELEMENT_DEFINE(DUST,1);
ELEMENT_DEFINE(WATR,2);
ELEMENT_DEFINE(GAS,3);


#undef ELEMENT_DEFINE
