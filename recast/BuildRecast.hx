package recast;

import idl.Options;

class BuildRecast extends idl.BuildBase{
	// Put any necessary includes in this string and they will be added to the generated files


	var options = {
		idlFile: "recast/recast.idl",
		target: null,
		packageName: "recast",
		nativeLib: "recast",
		outputDir: "src",
		includeCode: null,
		autoGC: true,
		defaultConfig: "Release",
		architecture: ArchAll
	};

	function new() {
		super(options);
	}

	override function getHLInclude() return '
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
			printf("C++ Detour: MAGIC=0x%X VERSION=0x%X\\n",
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

		static const int NAVMESHSET_MAGIC = \'MSET\';
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
	';


	public static function main() {
		var b = new BuildRecast();
		b.generate();

	}
}
