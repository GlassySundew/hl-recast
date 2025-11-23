#ifdef EMSCRIPTEN

#include <emscripten.h>
#define HL_PRIM
#define HL_NAME(n)	EMSCRIPTEN_KEEPALIVE eb_##n
#define DEFINE_PRIM(ret, name, args)
#define _OPT(t) t*
#define _GET_OPT(value,t) *value
#define alloc_ref(r, _) r
#define alloc_ref_const(r,_) r
#define _ref(t)			t
#define _unref(v)		v
#define free_ref(v) delete (v)
#define HL_CONST const

#else

#define HL_NAME(x) recast_##x
#include <hl.h>
#define _IDL _BYTES
#define _OPT(t) vdynamic *
#define _GET_OPT(value,t) (value)->v.t
template <typename T> struct pref {
	void *finalize;
	T *value;
};

#define _ref(t) pref<t>
#define _unref(v) v->value
#define alloc_ref(r,t) _alloc_ref(r,finalize_##t)
#define alloc_ref_const(r, _) _alloc_const(r)
#define HL_CONST

template<typename T> void free_ref( pref<T> *r ) {
	if( !r->finalize ) hl_error("delete() is not allowed on const value.");
	delete r->value;
	r->value = NULL;
	r->finalize = NULL;
}

template<typename T> pref<T> *_alloc_ref( T *value, void (*finalize)( pref<T> * ) ) {
	pref<T> *r = (pref<T>*)hl_gc_alloc_finalizer(sizeof(pref<T>));
	r->finalize = finalize;
	r->value = value;
	return r;
}

template<typename T> pref<T> *_alloc_const( const T *value ) {
	pref<T> *r = (pref<T>*)hl_gc_alloc_noptr(sizeof(pref<T>));
	r->finalize = NULL;
	r->value = (T*)value;
	return r;
}

#endif

#ifdef _WIN32
        #pragma warning(disable:4305)
        #pragma warning(disable:4244)
        #pragma warning(disable:4316)
        #endif

        #include <DetourNavMesh.h>
        #include <DetourNavMeshQuery.h>
        #include <DetourCommon.h>
        #include <DetourCrowd.h>
        #include <DetourTileCache.h>

        #include <Recast.h>
        #include <RecastAlloc.h>
        #include <RecastAssert.h>

extern "C" {

static void finalize_dtNavMeshParams( _ref(dtNavMeshParams)* _this ) { free_ref(_this); }
HL_PRIM void HL_NAME(dtNavMeshParams_delete)( _ref(dtNavMeshParams)* _this ) {
	free_ref(_this);
}
DEFINE_PRIM(_VOID, dtNavMeshParams_delete, _IDL);
static void finalize_dtNavMesh( _ref(dtNavMesh)* _this ) { free_ref(_this); }
HL_PRIM void HL_NAME(dtNavMesh_delete)( _ref(dtNavMesh)* _this ) {
	free_ref(_this);
}
DEFINE_PRIM(_VOID, dtNavMesh_delete, _IDL);
static void finalize_dtQueryFilter( _ref(dtQueryFilter)* _this ) { free_ref(_this); }
HL_PRIM void HL_NAME(dtQueryFilter_delete)( _ref(dtQueryFilter)* _this ) {
	free_ref(_this);
}
DEFINE_PRIM(_VOID, dtQueryFilter_delete, _IDL);
static void finalize_dtNavMeshQuery( _ref(dtNavMeshQuery)* _this ) { free_ref(_this); }
HL_PRIM void HL_NAME(dtNavMeshQuery_delete)( _ref(dtNavMeshQuery)* _this ) {
	free_ref(_this);
}
DEFINE_PRIM(_VOID, dtNavMeshQuery_delete, _IDL);
HL_PRIM float[] HL_NAME(dtNavMeshParams_get_orig)( _ref(dtNavMeshParams)* _this ) {
	return _unref(_this)->orig;
}
HL_PRIM float[] HL_NAME(dtNavMeshParams_set_orig)( _ref(dtNavMeshParams)* _this, float[] value ) {
	_unref(_this)->orig = (value);
	return value;
}
DEFINE_PRIM(_BYTES,dtNavMeshParams_get_orig,_IDL);
DEFINE_PRIM(_BYTES,dtNavMeshParams_set_orig,_IDL _BYTES);

HL_PRIM float HL_NAME(dtNavMeshParams_get_tileWidth)( _ref(dtNavMeshParams)* _this ) {
	return _unref(_this)->tileWidth;
}
HL_PRIM float HL_NAME(dtNavMeshParams_set_tileWidth)( _ref(dtNavMeshParams)* _this, float value ) {
	_unref(_this)->tileWidth = (value);
	return value;
}
DEFINE_PRIM(_F32,dtNavMeshParams_get_tileWidth,_IDL);
DEFINE_PRIM(_F32,dtNavMeshParams_set_tileWidth,_IDL _F32);

HL_PRIM float HL_NAME(dtNavMeshParams_get_tileHeight)( _ref(dtNavMeshParams)* _this ) {
	return _unref(_this)->tileHeight;
}
HL_PRIM float HL_NAME(dtNavMeshParams_set_tileHeight)( _ref(dtNavMeshParams)* _this, float value ) {
	_unref(_this)->tileHeight = (value);
	return value;
}
DEFINE_PRIM(_F32,dtNavMeshParams_get_tileHeight,_IDL);
DEFINE_PRIM(_F32,dtNavMeshParams_set_tileHeight,_IDL _F32);

HL_PRIM int HL_NAME(dtNavMeshParams_get_maxTiles)( _ref(dtNavMeshParams)* _this ) {
	return _unref(_this)->maxTiles;
}
HL_PRIM int HL_NAME(dtNavMeshParams_set_maxTiles)( _ref(dtNavMeshParams)* _this, int value ) {
	_unref(_this)->maxTiles = (value);
	return value;
}
DEFINE_PRIM(_I32,dtNavMeshParams_get_maxTiles,_IDL);
DEFINE_PRIM(_I32,dtNavMeshParams_set_maxTiles,_IDL _I32);

HL_PRIM int HL_NAME(dtNavMeshParams_get_maxPolys)( _ref(dtNavMeshParams)* _this ) {
	return _unref(_this)->maxPolys;
}
HL_PRIM int HL_NAME(dtNavMeshParams_set_maxPolys)( _ref(dtNavMeshParams)* _this, int value ) {
	_unref(_this)->maxPolys = (value);
	return value;
}
DEFINE_PRIM(_I32,dtNavMeshParams_get_maxPolys,_IDL);
DEFINE_PRIM(_I32,dtNavMeshParams_set_maxPolys,_IDL _I32);

HL_PRIM _ref(dtNavMesh)* HL_NAME(dtNavMesh_new0)() {
	return alloc_ref((new dtNavMesh()),dtNavMesh);
}
DEFINE_PRIM(_IDL, dtNavMesh_new0,);

HL_PRIM int HL_NAME(dtNavMesh_init1)(_ref(dtNavMesh)* _this, _ref(dtNavMeshParams)* params) {
	return _unref(_this)->init(*_unref(params));
}
DEFINE_PRIM(_I32, dtNavMesh_init1, _IDL _IDL);

HL_PRIM HL_CONST _ref(dtNavMeshParams)* HL_NAME(dtNavMesh_getParams0)(_ref(dtNavMesh)* _this) {
	return alloc_ref_const(&(_unref(_this)->getParams()),dtNavMeshParams);
}
DEFINE_PRIM(_IDL, dtNavMesh_getParams0, _IDL);

HL_PRIM int HL_NAME(dtNavMesh_addTile5)(_ref(dtNavMesh)* _this, unsigned char* data, int dataSize, int flags, int lastRef, int resultRef) {
	return _unref(_this)->addTile(data, dataSize, flags, lastRef, *resultRef);
}
DEFINE_PRIM(_I32, dtNavMesh_addTile5, _IDL _BYTES _I32 _I32 _I32 _I32);

HL_PRIM HL_CONST void HL_NAME(dtNavMesh_calcTileLoc3)(_ref(dtNavMesh)* _this, float* pos, int tx, int ty) {
	_unref(_this)->calcTileLoc(pos, *tx, *ty);
}
DEFINE_PRIM(_VOID, dtNavMesh_calcTileLoc3, _IDL _BYTES _I32 _I32);

HL_PRIM HL_CONST bool HL_NAME(dtNavMesh_isValidPolyRef1)(_ref(dtNavMesh)* _this, int ref) {
	return _unref(_this)->isValidPolyRef(ref);
}
DEFINE_PRIM(_BOOL, dtNavMesh_isValidPolyRef1, _IDL _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getMaxTiles0)(_ref(dtNavMesh)* _this) {
	return _unref(_this)->getMaxTiles();
}
DEFINE_PRIM(_I32, dtNavMesh_getMaxTiles0, _IDL);

HL_PRIM int HL_NAME(dtNavMesh_setPolyFlags2)(_ref(dtNavMesh)* _this, int ref, short flags) {
	return _unref(_this)->setPolyFlags(ref, flags);
}
DEFINE_PRIM(_I32, dtNavMesh_setPolyFlags2, _IDL _I32 _I16);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getPolyFlags2)(_ref(dtNavMesh)* _this, int ref, short flags) {
	return _unref(_this)->getPolyFlags(ref, *flags);
}
DEFINE_PRIM(_I32, dtNavMesh_getPolyFlags2, _IDL _I32 _I16);

HL_PRIM int HL_NAME(dtNavMesh_setPolyArea2)(_ref(dtNavMesh)* _this, int ref, short area) {
	return _unref(_this)->setPolyArea(ref, area);
}
DEFINE_PRIM(_I32, dtNavMesh_setPolyArea2, _IDL _I32 _I16);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getPolyArea2)(_ref(dtNavMesh)* _this, int ref, short area) {
	return _unref(_this)->getPolyArea(ref, *area);
}
DEFINE_PRIM(_I32, dtNavMesh_getPolyArea2, _IDL _I32 _I16);

HL_PRIM _ref(dtQueryFilter)* HL_NAME(dtQueryFilter_new0)() {
	return alloc_ref((new dtQueryFilter()),dtQueryFilter);
}
DEFINE_PRIM(_IDL, dtQueryFilter_new0,);

HL_PRIM HL_CONST float HL_NAME(dtQueryFilter_getAreaCost1)(_ref(dtQueryFilter)* _this, int i) {
	return _unref(_this)->getAreaCost(i);
}
DEFINE_PRIM(_F32, dtQueryFilter_getAreaCost1, _IDL _I32);

HL_PRIM void HL_NAME(dtQueryFilter_setAreaCost2)(_ref(dtQueryFilter)* _this, int i, float cost) {
	_unref(_this)->setAreaCost(i, cost);
}
DEFINE_PRIM(_VOID, dtQueryFilter_setAreaCost2, _IDL _I32 _F32);

HL_PRIM HL_CONST short HL_NAME(dtQueryFilter_getIncludeFlags0)(_ref(dtQueryFilter)* _this) {
	return _unref(_this)->getIncludeFlags();
}
DEFINE_PRIM(_I16, dtQueryFilter_getIncludeFlags0, _IDL);

HL_PRIM void HL_NAME(dtQueryFilter_setIncludeFlags1)(_ref(dtQueryFilter)* _this, short flags) {
	_unref(_this)->setIncludeFlags(flags);
}
DEFINE_PRIM(_VOID, dtQueryFilter_setIncludeFlags1, _IDL _I16);

HL_PRIM HL_CONST short HL_NAME(dtQueryFilter_getExcludeFlags0)(_ref(dtQueryFilter)* _this) {
	return _unref(_this)->getExcludeFlags();
}
DEFINE_PRIM(_I16, dtQueryFilter_getExcludeFlags0, _IDL);

HL_PRIM void HL_NAME(dtQueryFilter_setExcludeFlags1)(_ref(dtQueryFilter)* _this, short flags) {
	_unref(_this)->setExcludeFlags(flags);
}
DEFINE_PRIM(_VOID, dtQueryFilter_setExcludeFlags1, _IDL _I16);

HL_PRIM _ref(dtNavMeshQuery)* HL_NAME(dtNavMeshQuery_new0)() {
	return alloc_ref((new dtNavMeshQuery()),dtNavMeshQuery);
}
DEFINE_PRIM(_IDL, dtNavMeshQuery_new0,);

HL_PRIM int HL_NAME(dtNavMeshQuery_init2)(_ref(dtNavMeshQuery)* _this, _ref(dtNavMesh)* nav, int maxNodes) {
	return _unref(_this)->init(*_unref(nav), maxNodes);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_init2, _IDL _IDL _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findPath8)(_ref(dtNavMeshQuery)* _this, int startRef, int endRef, float* startPos, float* endPos, _ref(dtQueryFilter)* filter, int* path, int pathCount, int maxPath) {
	return _unref(_this)->findPath(startRef, endRef, startPos, endPos, *_unref(filter), *path, *pathCount, maxPath);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findPath8, _IDL _I32 _I32 _BYTES _BYTES _IDL _BYTES _I32 _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findStraightPath9)(_ref(dtNavMeshQuery)* _this, float* startPos, float* endPos, int* path, int pathSize, float* straightPath, short* straightPathFlags, int* straightPathRefs, int straightPathCount, int maxStraightPath) {
	return _unref(_this)->findStraightPath(startPos, endPos, path, pathSize, *straightPath, *straightPathFlags, *straightPathRefs, *straightPathCount, maxStraightPath);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findStraightPath9, _IDL _BYTES _BYTES _BYTES _I32 _BYTES _BYTES _BYTES _I32 _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findNearestPoly5)(_ref(dtNavMeshQuery)* _this, float* center, float* extents, _ref(dtQueryFilter)* filter, int nearestRef, float* nearestPt) {
	return _unref(_this)->findNearestPoly(center, extents, *_unref(filter), *nearestRef, *nearestPt);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findNearestPoly5, _IDL _BYTES _BYTES _IDL _I32 _BYTES);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_moveAlongSurface8)(_ref(dtNavMeshQuery)* _this, int startRef, float* startPos, float* endPos, _ref(dtQueryFilter)* filter, float* resultPos, int* visited, int visitedCount, int maxVisitedSize) {
	return _unref(_this)->moveAlongSurface(startRef, startPos, endPos, *_unref(filter), *resultPos, *visited, *visitedCount, maxVisitedSize);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_moveAlongSurface8, _IDL _I32 _BYTES _BYTES _IDL _BYTES _BYTES _I32 _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_getPolyHeight3)(_ref(dtNavMeshQuery)* _this, int ref, float* pos, float height) {
	return _unref(_this)->getPolyHeight(ref, pos, *height);
}
DEFINE_PRIM(_I32, dtNavMeshQuery_getPolyHeight3, _IDL _I32 _BYTES _F32);

HL_PRIM HL_CONST bool HL_NAME(dtNavMeshQuery_isValidPolyRef2)(_ref(dtNavMeshQuery)* _this, int ref, _ref(dtQueryFilter)* filter) {
	return _unref(_this)->isValidPolyRef(ref, *_unref(filter));
}
DEFINE_PRIM(_BOOL, dtNavMeshQuery_isValidPolyRef2, _IDL _I32 _IDL);

HL_PRIM HL_CONST _ref(dtNavMesh)* HL_NAME(dtNavMeshQuery_getAttachedNavMesh0)(_ref(dtNavMeshQuery)* _this) {
	return alloc_ref_const(&(_unref(_this)->getAttachedNavMesh()),dtNavMesh);
}
DEFINE_PRIM(_IDL, dtNavMeshQuery_getAttachedNavMesh0, _IDL);

}
