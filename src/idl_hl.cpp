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
#include "hl-idl-helpers.hpp"
// Need to link in helpers
//HL_API hl_type hltx_ui16;
//HL_API hl_type hltx_ui8;
HL_PRIM hl_type hltx_ui16 = { HUI16 };
HL_PRIM hl_type hltx_ui8 = { HUI8 };

#define _IDL _BYTES
#define _OPT(t) vdynamic *
#define _GET_OPT(value,t) (value)->v.t

static  hl_type *strType = nullptr;
void hl_cache_string_type( vstring *str) {
   strType = str->t;
}

vstring * hl_utf8_to_hlstr( const char *str) {
    int strLen = (int)strlen( str );
    uchar * ubuf = (uchar*)hl_gc_alloc_noptr((strLen + 1) << 1);
    hl_from_utf8( ubuf, strLen, str );

    vstring* vstr = (vstring *)hl_gc_alloc_raw(sizeof(vstring));

    vstr->bytes = ubuf;
    vstr->length = strLen;
    vstr->t = strType;
    return vstr;
}
vstring * hl_utf8_to_hlstr( const std::string &str) {
	return hl_utf8_to_hlstr(str.c_str());
}

HL_PRIM vstring * HL_NAME(getdllversion)(vstring * haxeversion) {
	strType = haxeversion->t;
	return haxeversion;
}
DEFINE_PRIM(_STRING, getdllversion, _STRING);

class HNativeBuffer {
    unsigned char *_ptr;
    int _size;

   public:
   inline unsigned char * ptr() { return _ptr; }
   inline int size() { return _size; }
   HNativeBuffer(unsigned char *ptr, int size) : _ptr(ptr), _size(size) {}
   HNativeBuffer(int size) : _ptr(new unsigned char[size]), _size(size) {}
    ~HNativeBuffer() {
        if (_ptr != nullptr)
            delete [] _ptr;
    }
};

inline static varray* _idc_alloc_array(float *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	float *p;
	a = hl_alloc_array(&hlt_f32, count);
	p = hl_aptr(a, float);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}
template <typename T> struct pref {
	void (*finalize)( pref<T> * );
	T *value;
};

#define _ref(t) pref<t>
#define _unref(v) v->value
#define _unref_ptr_safe(v) (v != nullptr ? v->value : nullptr)
#define alloc_ref(r,t) _alloc_ref(r,finalize_##t)
#define alloc_ref_const(r, _) _alloc_const(r)
#define HL_CONST

template<typename T> void free_ref( pref<T> *r ) {
	if( !r->finalize ) hl_error("delete() is not allowed on const value.");
	delete r->value;
	r->value = NULL;
	r->finalize = NULL;
}

template<typename T> void free_ref( pref<T> *r, void (*deleteFunc)(T*) ) {
	if( !r->finalize ) hl_error("delete() is not allowed on const value.");
	deleteFunc( r->value );
	r->value = NULL;
	r->finalize = NULL;
}

inline void testvector(_h_float3 *v) {
  printf("v: %f %f %f\n", v->x, v->y, v->z);
}
template<typename T> pref<T> *_alloc_ref( T *value, void (*finalize)( pref<T> * ) ) {
	if (value == nullptr) return nullptr;
	pref<T> *r = (pref<T>*)hl_gc_alloc_finalizer(sizeof(pref<T>));
	r->finalize = finalize;
	r->value = value;
	return r;
}

template<typename T> pref<T> *_alloc_const( const T *value ) {
	if (value == nullptr) return nullptr;
	pref<T> *r = (pref<T>*)hl_gc_alloc_noptr(sizeof(pref<T>));
	r->finalize = NULL;
	r->value = (T*)value;
	return r;
}


inline static varray* _idc_alloc_array(unsigned char *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	float *p;
	a = hl_alloc_array(&hltx_ui8, count);
	p = hl_aptr(a, float);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static varray* _idc_alloc_array( char *src, int count) {
	return _idc_alloc_array((unsigned char *)src, count);
}

inline static varray* _idc_alloc_array(int *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	int *p;
	a = hl_alloc_array(&hlt_i32, count);
	p = hl_aptr(a, int);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;

}

inline static varray* _idc_alloc_array(double *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	double *p;
	a = hl_alloc_array(&hlt_f64, count);
	p = hl_aptr(a, double);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}


inline static varray* _idc_alloc_array(const unsigned short *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	unsigned short *p;
	a = hl_alloc_array(&hltx_ui16, count);
	p = hl_aptr(a, unsigned short);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static varray* _idc_alloc_array(unsigned short *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	unsigned short *p;
	a = hl_alloc_array(&hltx_ui16, count);
	p = hl_aptr(a, unsigned short);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static void _idc_copy_array( float *dst, varray *src, int count) {
	float *p = hl_aptr(src, float);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, float *src,  int count) {
	float *p = hl_aptr(dst, float);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}


inline static void _idc_copy_array( int *dst, varray *src, int count) {
	int *p = hl_aptr(src, int);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( unsigned short *dst, varray *src) {
	unsigned short *p = hl_aptr(src, unsigned short);
	for (int i = 0; i < src->size; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( const unsigned short *cdst, varray *src) {
	unsigned short *p = hl_aptr(src, unsigned short);
	unsigned short *dst = (unsigned short *)cdst;
	for (int i = 0; i < src->size; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, int *src,  int count) {
	int *p = hl_aptr(dst, int);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}


inline static void _idc_copy_array( double *dst, varray *src, int count) {
	double *p = hl_aptr(src, double);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, double *src,  int count) {
	double *p = hl_aptr(dst, double);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}

#endif

#ifdef _WIN32
		#pragma warning(disable:4305)
		#pragma warning(disable:4244)
		#pragma warning(disable:4316)
		#endif
		#include <Recast.h>
		#include <RecastAlloc.h>
		#include <RecastAssert.h>
		#include "ChunkyTriMesh.h"

		#include <DetourAlloc.h>
		#include <DetourAssert.h>
		#include <DetourCommon.h>
		#include <DetourMath.h>
		#include <DetourNavMesh.h>
		#include <DetourNavMeshBuilder.h>
		#include <DetourNavMeshQuery.h>
		#include <DetourNode.h>
		#include <DetourStatus.h>
		#include <DetourTileCache.h>
		#include <DetourTileCacheBuilder.h>
		#include "hl-recast.h"

		class Recast;
		class DetourMath;
		class DetourCommon;
		class DetourStatus;
		class DetourNavMesh;
		class DetourNavMeshBuilder;
		class DetourNavMeshQuery;

		bool rcHeightfield_rcSpanIsValidAt(rcHeightfield *context, int pos) {
			return context->spans[pos] ? true : false;
		}

		rcSpan* rcHeightfield_rcSpanAt(rcHeightfield *context, int pos) {
			return context->spans[pos];
		}

		HL_PRIM void HL_NAME(debugPrintDetourVersionCpp)() {
			printf("C++ Detour: MAGIC=0x%X VERSION=0x%X\n",
				DT_NAVMESH_MAGIC,
				DT_NAVMESH_VERSION
			);
		}
		DEFINE_PRIM(_VOID, debugPrintDetourVersionCpp, _NO_ARG);

		struct RecastHeader {
			int magic;
			int version;
			int numTiles;
		};

		struct NavMeshSetHeader {
			dtNavMeshParams params;
		};

		struct NavMeshTileHeader {
			dtTileRef tileRef;
			int dataSize;
		};

		static const int NAVMESHSET_MAGIC = 'MSET';
		static const int NAVMESHSET_VERSION = 1;

		HL_PRIM int HL_NAME(dtNavMesh_initFromSet)(
			pref<dtNavMesh>* _this,
			vbyte* bytes,
			int dataSize
			) {
			unsigned char* bits = (unsigned char*)bytes;
			unsigned char* end = bits + dataSize;

			if (dataSize < (int)sizeof(RecastHeader) + (int)sizeof(NavMeshSetHeader))
				return (int)DT_INVALID_PARAM;

			RecastHeader recastHeader;
			memcpy(&recastHeader, bits, sizeof(recastHeader));
			bits += sizeof(recastHeader);

			if (recastHeader.magic != NAVMESHSET_MAGIC || recastHeader.version != NAVMESHSET_VERSION)
				return (int)(DT_FAILURE | DT_WRONG_MAGIC);

			NavMeshSetHeader header;
			memcpy(&header, bits, sizeof(header));
			bits += sizeof(header);

			dtNavMesh* mesh = _unref(_this);
			dtStatus status = mesh->init(&header.params);
			if (dtStatusFailed(status))
				return (int)status;

			for (int i = 0; i < recastHeader.numTiles; ++i) {
				if (bits + sizeof(NavMeshTileHeader) > end)
					break;

				NavMeshTileHeader tileHeader;
				memcpy(&tileHeader, bits, sizeof(tileHeader));
				bits += sizeof(tileHeader);

				if (!tileHeader.tileRef || !tileHeader.dataSize)
					break;

				if (bits + tileHeader.dataSize > end)
					break;

				unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
				if (!data)
					return (int)(DT_FAILURE | DT_OUT_OF_MEMORY);

				memcpy(data, bits, tileHeader.dataSize);
				bits += tileHeader.dataSize;

				dtTileRef outRef = 0;
				dtStatus addStatus = mesh->addTile(
					data,
					tileHeader.dataSize,
					DT_TILE_FREE_DATA,   // Detour owns and frees the tile data
					tileHeader.tileRef,
					&outRef
				);

				if (dtStatusFailed(addStatus)) {
					dtFree(data);
					return (int)addStatus;
				}
			}

			return (int)DT_SUCCESS;
		}
		DEFINE_PRIM(_I32, dtNavMesh_initFromSet, _IDL _BYTES _I32);





extern "C" {

static void finalize_dtNavMeshParams( pref<dtNavMeshParams>* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(dtNavMeshParams_delete)( pref<dtNavMeshParams>* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, dtNavMeshParams_delete, _IDL);
static void finalize_dtNavMesh( pref<dtNavMesh>* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(dtNavMesh_delete)( pref<dtNavMesh>* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, dtNavMesh_delete, _IDL);
static void finalize_dtQueryFilter( pref<dtQueryFilter>* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(dtQueryFilter_delete)( pref<dtQueryFilter>* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, dtQueryFilter_delete, _IDL);
static void finalize_dtNavMeshQuery( pref<dtNavMeshQuery>* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(dtNavMeshQuery_delete)( pref<dtNavMeshQuery>* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, dtNavMeshQuery_delete, _IDL);
HL_PRIM h_float3 HL_NAME(dtNavMeshParams_get_orig)( pref<dtNavMeshParams>* _this ) {
	return (h_float3 )(_unref(_this)->orig);
}
HL_PRIM void HL_NAME(dtNavMeshParams_getorigv)( pref<dtNavMeshParams>* _this, h_float3 value ) {
	 float *src = (float*) & (_unref(_this)->orig)[0];
	 float *dst = (float*) value;
	dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
}
DEFINE_PRIM(_VOID,dtNavMeshParams_getorigv,_IDL _STRUCT  );
DEFINE_PRIM(_STRUCT,dtNavMeshParams_get_orig,_IDL);
HL_PRIM h_float3 HL_NAME(dtNavMeshParams_set_orig)( pref<dtNavMeshParams>* _this, h_float3 value ) {
	 float *dst = (float*) & (_unref(_this)->orig)[0];
	 float *src = (float*) value;
	dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
	return value;
}
HL_PRIM void HL_NAME(dtNavMeshParams_setorig3)( pref<dtNavMeshParams>* _this,  float value0, float value1, float value2 ) {
	 float *p = (_unref(_this)->orig);
	p[0] = value0; p[1] = value1; p[2] = value2;
}
DEFINE_PRIM(_VOID,dtNavMeshParams_setorig3,_IDL _F32 _F32 _F32 );
DEFINE_PRIM(_STRUCT,dtNavMeshParams_set_orig,_IDL _STRUCT);

HL_PRIM float HL_NAME(dtNavMeshParams_get_tileWidth)( pref<dtNavMeshParams>* _this ) {
	return _unref(_this)->tileWidth;
}
DEFINE_PRIM(_F32,dtNavMeshParams_get_tileWidth,_IDL);
HL_PRIM float HL_NAME(dtNavMeshParams_set_tileWidth)( pref<dtNavMeshParams>* _this, float value ) {
	_unref(_this)->tileWidth = (value);
	return value;
}
DEFINE_PRIM(_F32,dtNavMeshParams_set_tileWidth,_IDL _F32);

HL_PRIM float HL_NAME(dtNavMeshParams_get_tileHeight)( pref<dtNavMeshParams>* _this ) {
	return _unref(_this)->tileHeight;
}
DEFINE_PRIM(_F32,dtNavMeshParams_get_tileHeight,_IDL);
HL_PRIM float HL_NAME(dtNavMeshParams_set_tileHeight)( pref<dtNavMeshParams>* _this, float value ) {
	_unref(_this)->tileHeight = (value);
	return value;
}
DEFINE_PRIM(_F32,dtNavMeshParams_set_tileHeight,_IDL _F32);

HL_PRIM int HL_NAME(dtNavMeshParams_get_maxTiles)( pref<dtNavMeshParams>* _this ) {
	return _unref(_this)->maxTiles;
}
DEFINE_PRIM(_I32,dtNavMeshParams_get_maxTiles,_IDL);
HL_PRIM int HL_NAME(dtNavMeshParams_set_maxTiles)( pref<dtNavMeshParams>* _this, int value ) {
	_unref(_this)->maxTiles = (value);
	return value;
}
DEFINE_PRIM(_I32,dtNavMeshParams_set_maxTiles,_IDL _I32);

HL_PRIM int HL_NAME(dtNavMeshParams_get_maxPolys)( pref<dtNavMeshParams>* _this ) {
	return _unref(_this)->maxPolys;
}
DEFINE_PRIM(_I32,dtNavMeshParams_get_maxPolys,_IDL);
HL_PRIM int HL_NAME(dtNavMeshParams_set_maxPolys)( pref<dtNavMeshParams>* _this, int value ) {
	_unref(_this)->maxPolys = (value);
	return value;
}
DEFINE_PRIM(_I32,dtNavMeshParams_set_maxPolys,_IDL _I32);

HL_PRIM pref<dtNavMesh>* HL_NAME(dtNavMesh_new0)() {
	return alloc_ref((new dtNavMesh()),dtNavMesh);
}
DEFINE_PRIM(_IDL, dtNavMesh_new0,);

HL_PRIM int HL_NAME(dtNavMesh_init1)(pref<dtNavMesh>* _this, pref<dtNavMeshParams>* params) {
	return (_unref(_this)->init(_unref_ptr_safe(params)));
}
DEFINE_PRIM(_I32, dtNavMesh_init1, _IDL _IDL);

HL_PRIM int HL_NAME(dtNavMesh_init3)(pref<dtNavMesh>* _this, vbyte* data, int dataSize, int flags) {
	return (_unref(_this)->init(data, dataSize, flags));
}
DEFINE_PRIM(_I32, dtNavMesh_init3, _IDL _BYTES _I32 _I32);

HL_PRIM HL_CONST pref<dtNavMeshParams>* HL_NAME(dtNavMesh_getParams0)(pref<dtNavMesh>* _this) {
	return alloc_ref_const((_unref(_this)->getParams()),dtNavMeshParams);
}
DEFINE_PRIM(_IDL, dtNavMesh_getParams0, _IDL);

HL_PRIM int HL_NAME(dtNavMesh_addTile5)(pref<dtNavMesh>* _this, varray* data, int dataSize, int flags, int lastRef, unsigned int* resultRef) {
	return (_unref(_this)->addTile(hl_aptr(data,unsigned char), dataSize, flags, lastRef, resultRef));
}
DEFINE_PRIM(_I32, dtNavMesh_addTile5, _IDL _ARR _I32 _I32 _I32 _REF(_I32));

HL_PRIM HL_CONST void HL_NAME(dtNavMesh_calcTileLoc3)(pref<dtNavMesh>* _this, varray* pos, int* tx, int* ty) {
	(_unref(_this)->calcTileLoc(hl_aptr(pos,float), tx, ty));
}
DEFINE_PRIM(_VOID, dtNavMesh_calcTileLoc3, _IDL _ARR _REF(_I32) _REF(_I32));

HL_PRIM HL_CONST bool HL_NAME(dtNavMesh_isValidPolyRef1)(pref<dtNavMesh>* _this, int ref) {
	return (_unref(_this)->isValidPolyRef(ref));
}
DEFINE_PRIM(_BOOL, dtNavMesh_isValidPolyRef1, _IDL _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getMaxTiles0)(pref<dtNavMesh>* _this) {
	return (_unref(_this)->getMaxTiles());
}
DEFINE_PRIM(_I32, dtNavMesh_getMaxTiles0, _IDL);

HL_PRIM int HL_NAME(dtNavMesh_setPolyFlags2)(pref<dtNavMesh>* _this, int ref, unsigned short flags) {
	return (_unref(_this)->setPolyFlags(ref, flags));
}
DEFINE_PRIM(_I32, dtNavMesh_setPolyFlags2, _IDL _I32 _I16);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getPolyFlags2)(pref<dtNavMesh>* _this, int ref, unsigned short* flags) {
	return (_unref(_this)->getPolyFlags(ref, flags));
}
DEFINE_PRIM(_I32, dtNavMesh_getPolyFlags2, _IDL _I32 _REF(_I16));

HL_PRIM int HL_NAME(dtNavMesh_setPolyArea2)(pref<dtNavMesh>* _this, int ref, unsigned short area) {
	return (_unref(_this)->setPolyArea(ref, area));
}
DEFINE_PRIM(_I32, dtNavMesh_setPolyArea2, _IDL _I32 _I16);

HL_PRIM HL_CONST int HL_NAME(dtNavMesh_getPolyArea2)(pref<dtNavMesh>* _this, int ref, unsigned char* area) {
	return (_unref(_this)->getPolyArea(ref, area));
}
DEFINE_PRIM(_I32, dtNavMesh_getPolyArea2, _IDL _I32 _REF(_I8));

HL_PRIM pref<dtQueryFilter>* HL_NAME(dtQueryFilter_new0)() {
	return alloc_ref((new dtQueryFilter()),dtQueryFilter);
}
DEFINE_PRIM(_IDL, dtQueryFilter_new0,);

HL_PRIM HL_CONST float HL_NAME(dtQueryFilter_getAreaCost1)(pref<dtQueryFilter>* _this, int i) {
	return (_unref(_this)->getAreaCost(i));
}
DEFINE_PRIM(_F32, dtQueryFilter_getAreaCost1, _IDL _I32);

HL_PRIM void HL_NAME(dtQueryFilter_setAreaCost2)(pref<dtQueryFilter>* _this, int i, float cost) {
	(_unref(_this)->setAreaCost(i, cost));
}
DEFINE_PRIM(_VOID, dtQueryFilter_setAreaCost2, _IDL _I32 _F32);

HL_PRIM HL_CONST unsigned short HL_NAME(dtQueryFilter_getIncludeFlags0)(pref<dtQueryFilter>* _this) {
	return (_unref(_this)->getIncludeFlags());
}
DEFINE_PRIM(_I16, dtQueryFilter_getIncludeFlags0, _IDL);

HL_PRIM void HL_NAME(dtQueryFilter_setIncludeFlags1)(pref<dtQueryFilter>* _this, unsigned short flags) {
	(_unref(_this)->setIncludeFlags(flags));
}
DEFINE_PRIM(_VOID, dtQueryFilter_setIncludeFlags1, _IDL _I16);

HL_PRIM HL_CONST unsigned short HL_NAME(dtQueryFilter_getExcludeFlags0)(pref<dtQueryFilter>* _this) {
	return (_unref(_this)->getExcludeFlags());
}
DEFINE_PRIM(_I16, dtQueryFilter_getExcludeFlags0, _IDL);

HL_PRIM void HL_NAME(dtQueryFilter_setExcludeFlags1)(pref<dtQueryFilter>* _this, unsigned short flags) {
	(_unref(_this)->setExcludeFlags(flags));
}
DEFINE_PRIM(_VOID, dtQueryFilter_setExcludeFlags1, _IDL _I16);

HL_PRIM pref<dtNavMeshQuery>* HL_NAME(dtNavMeshQuery_new0)() {
	return alloc_ref((new dtNavMeshQuery()),dtNavMeshQuery);
}
DEFINE_PRIM(_IDL, dtNavMeshQuery_new0,);

HL_PRIM int HL_NAME(dtNavMeshQuery_init2)(pref<dtNavMeshQuery>* _this, pref<dtNavMesh>* nav, int maxNodes) {
	return (_unref(_this)->init(_unref_ptr_safe(nav), maxNodes));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_init2, _IDL _IDL _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findPath8)(pref<dtNavMeshQuery>* _this, int startRef, int endRef, varray* startPos, varray* endPos, pref<dtQueryFilter>* filter, varray* path, int* pathCount, int maxPath) {
	return (_unref(_this)->findPath(startRef, endRef, hl_aptr(startPos,float), hl_aptr(endPos,float), _unref_ptr_safe(filter), hl_aptr(path,unsigned int), pathCount, maxPath));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findPath8, _IDL _I32 _I32 _ARR _ARR _IDL _ARR _REF(_I32) _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findStraightPath9)(pref<dtNavMeshQuery>* _this, varray* startPos, varray* endPos, varray* path, int pathSize, varray* straightPath, varray* straightPathFlags, varray* straightPathRefs, int* straightPathCount, int maxStraightPath) {
	return (_unref(_this)->findStraightPath(hl_aptr(startPos,float), hl_aptr(endPos,float), hl_aptr(path,unsigned int), pathSize, hl_aptr(straightPath,float), hl_aptr(straightPathFlags,unsigned char), hl_aptr(straightPathRefs,unsigned int), straightPathCount, maxStraightPath));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findStraightPath9, _IDL _ARR _ARR _ARR _I32 _ARR _ARR _ARR _REF(_I32) _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_findNearestPoly5)(pref<dtNavMeshQuery>* _this, varray* center, varray* extents, pref<dtQueryFilter>* filter, unsigned int* nearestRef, varray* nearestPt) {
	return (_unref(_this)->findNearestPoly(hl_aptr(center,float), hl_aptr(extents,float), _unref_ptr_safe(filter), nearestRef, hl_aptr(nearestPt,float)));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_findNearestPoly5, _IDL _ARR _ARR _IDL _REF(_I32) _ARR);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_moveAlongSurface8)(pref<dtNavMeshQuery>* _this, int startRef, varray* startPos, varray* endPos, pref<dtQueryFilter>* filter, varray* resultPos, varray* visited, int* visitedCount, int maxVisitedSize) {
	return (_unref(_this)->moveAlongSurface(startRef, hl_aptr(startPos,float), hl_aptr(endPos,float), _unref_ptr_safe(filter), hl_aptr(resultPos,float), hl_aptr(visited,unsigned int), visitedCount, maxVisitedSize));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_moveAlongSurface8, _IDL _I32 _ARR _ARR _IDL _ARR _ARR _REF(_I32) _I32);

HL_PRIM HL_CONST int HL_NAME(dtNavMeshQuery_getPolyHeight3)(pref<dtNavMeshQuery>* _this, int ref, varray* pos, float* height) {
	return (_unref(_this)->getPolyHeight(ref, hl_aptr(pos,float), height));
}
DEFINE_PRIM(_I32, dtNavMeshQuery_getPolyHeight3, _IDL _I32 _ARR _REF(_F32));

HL_PRIM HL_CONST bool HL_NAME(dtNavMeshQuery_isValidPolyRef2)(pref<dtNavMeshQuery>* _this, int ref, pref<dtQueryFilter>* filter) {
	return (_unref(_this)->isValidPolyRef(ref, _unref_ptr_safe(filter)));
}
DEFINE_PRIM(_BOOL, dtNavMeshQuery_isValidPolyRef2, _IDL _I32 _IDL);

HL_PRIM HL_CONST pref<dtNavMesh>* HL_NAME(dtNavMeshQuery_getAttachedNavMesh0)(pref<dtNavMeshQuery>* _this) {
	return alloc_ref_const((_unref(_this)->getAttachedNavMesh()),dtNavMesh);
}
DEFINE_PRIM(_IDL, dtNavMeshQuery_getAttachedNavMesh0, _IDL);

}
