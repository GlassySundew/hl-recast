package recast;

import haxe.io.Bytes;
import hl.NativeArray;
import hl.Ref;
import h3d.col.Point;
import recast.DetourStatus;
import recast.Recast;

enum PathStatus {

	NOT_LOADED;
	INVALID_START_OR_END;
	NO_PATH;
	OK;
}

typedef PathResult = {
	final status : PathStatus;
	@:optional final statusList : Array<StatusType>;
	@:optional final path : Array<Point>;
};

class NavMeshWrapper {

	static final EXT_X = 2;
	static final EXT_Y = 25;
	static final EXT_Z = 2;

	static final MAX_PATH = 256;

	final inst : NavMesh;
	final query : NavMeshQuery;

	// buff allocations
	final filter = new recast.Recast.QueryFilter();
	final polys = new NativeArray<Int>( MAX_PATH );
	final straight = new NativeArray<Single>( 3 * MAX_PATH );
	final straightFlags = new NativeArray<hl.UI8>( MAX_PATH );
	final straightRefs = new NativeArray<Int>( MAX_PATH );

	final startPtr = new NativeArray<Single>( 3 );
	final endPtr = new NativeArray<Single>( 3 );
	final ext = new NativeArray<Single>( 3 );

	final nearestStart = new NativeArray<Single>( 3 );
	final nearestEnd = new NativeArray<Single>( 3 );

	public function new( bytes : Bytes ) {

		inst = new NavMesh();

		final initResult = RecastStatic.initNavMeshFromSet(
			inst, bytes.getData().bytes, bytes.length
		);
		final initSt = DetourStatus.fromInt( initResult );

		#if debug
		if ( !initSt.contains( SUCCESS ) )
			trace( "error while nav mesh init: " + initSt );
		#end

		query = new NavMeshQuery();

		final stQueryInit = query.init( inst, 2048 );
		final queryInitSt = DetourStatus.fromInt( stQueryInit );

		#if debug
		if ( !queryInitSt.contains( SUCCESS ) )
			trace( "error while nav mesh query init: " + queryInitSt );
		#end
	}

	public function findPath(
		start : Point,
		end : Point,
		?searchExtents : Point
	) : PathResult {

		var status : PathStatus = OK;
		var statusList : Array<StatusType> = null;
		if ( searchExtents == null )
			searchExtents = new Point( EXT_X, EXT_Z, EXT_Y );

		// recast y up
		startPtr[0] = start.x;
		startPtr[1] = start.z;
		startPtr[2] = -start.y;

		endPtr[0] = end.x;
		endPtr[1] = end.z;
		endPtr[2] = -end.y;

		ext[0] = searchExtents.x;
		ext[1] = searchExtents.y;
		ext[2] = searchExtents.z;

		var startVal = 0;
		final startRef = new Ref<Int>( startVal );
		var endVal = 0;
		final endRef = new Ref<Int>( endVal );

		var st = query.findNearestPoly( startPtr, ext, filter, startRef, nearestStart );

		st = query.findNearestPoly( endPtr, ext, filter, endRef, nearestEnd );

		if ( startRef.get() == 0 || endRef.get() == 0 ) {

			status = INVALID_START_OR_END;
		}

		var pathCountVal = 0;
		final pathCountRef = new Ref<Int>( pathCountVal );

		st = query.findPath(
			startRef.get(),
			endRef.get(),
			nearestStart,
			nearestEnd,
			filter,
			polys,
			pathCountRef,
			MAX_PATH
		);
		final findPathSt = DetourStatus.fromInt( st );

		#if debug
		if ( !findPathSt.contains( SUCCESS ) ) {

			statusList = findPathSt;
			status = NO_PATH;
		}
		#end

		final maxStraight = MAX_PATH;
		var straightCountVal = 0;
		final straightCountRef = new Ref<Int>( straightCountVal );
		final polyCount = pathCountRef.get();

		st = query.findStraightPath(
			nearestStart,
			nearestEnd,
			polys,
			polyCount,
			straight,
			straightFlags,
			straightRefs,
			straightCountRef,
			maxStraight
		);

		final straightCount = straightCountRef.get();

		final out : Array<Point> = [];
		out.resize( straightCount );
		for ( i in 0...straightCount ) {

			final rx = straight[i * 3 + 0];
			final ry = straight[i * 3 + 1];
			final rz = straight[i * 3 + 2];

			out[i] = new Point( rx, -rz, ry );
		}

		return { status : status, statusList : statusList, path : out };
	}
}
