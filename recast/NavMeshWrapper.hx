package recast;

import haxe.io.Bytes;
import hl.NativeArray;
import hl.Ref;
import h3d.col.Point;
import recast.Recast.RecastStatic;
import recast.Recast;

class NavMeshWrapper {

	static final EXT_X = 2;
	static final EXT_Y = 4;
	static final EXT_Z = 2;

	static final MAX_PATH = 256;

	final inst : NavMesh;
	final query : NavMeshQuery;

	public function new( bytes : Bytes ) {

		inst = new NavMesh();

		final initResult = RecastStatic.initNavMeshFromSet(
			inst, bytes.getData().bytes, bytes.length
		);
		final initSt = DetourStatus.fromInt( initResult );
		if ( !initSt.contains( SUCCESS ) )
			trace( "error while nav mesh init: " + initSt );

		query = new NavMeshQuery();

		final stQueryInit = query.init( inst, 2048 );
		final queryInitSt = DetourStatus.fromInt( stQueryInit );
		if ( !queryInitSt.contains( SUCCESS ) )
			trace( "error while nav mesh query init: " + queryInitSt );
	}

	public function findPath( start : Point, end : Point ) : Array<Point> {

		final startPtr = new NativeArray<Single>( 3 );
		final endPtr = new NativeArray<Single>( 3 );
		final ext = new NativeArray<Single>( 3 );

		// recast y up
		startPtr[0] = start.x;
		startPtr[1] = start.z;
		startPtr[2] = -start.y;

		endPtr[0] = end.x;
		endPtr[1] = end.z;
		endPtr[2] = -end.y;

		ext[0] = EXT_X;
		ext[1] = EXT_Y;
		ext[2] = EXT_Z;

		final filter = new recast.Recast.QueryFilter();

		var startVal = 0;
		final startRef = new Ref<Int>( startVal );
		var endVal = 0;
		final endRef = new Ref<Int>( endVal );

		final nearestStart = new NativeArray<Single>( 3 );
		final nearestEnd = new NativeArray<Single>( 3 );

		var st = query.findNearestPoly( startPtr, ext, filter, startRef, nearestStart );
		trace( 'findNearestPoly(start) st=$st ref=${startRef.get()}' );

		st = query.findNearestPoly( endPtr, ext, filter, endRef, nearestEnd );
		trace( 'findNearestPoly(end) st=$st ref=${endRef.get()}' );

		if ( startRef.get() == 0 || endRef.get() == 0 ) {

			trace( 'No valid start or end poly' );
		}

		final polys = new NativeArray<Int>( MAX_PATH );
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
		if ( !findPathSt.contains( SUCCESS ) )
			trace( "error while finding path: " + findPathSt );

		final maxStraight = MAX_PATH;
		final straight = new NativeArray<Single>( 3 * maxStraight );
		final straightFlags = new NativeArray<hl.UI8>( maxStraight );
		final straightRefs = new NativeArray<Int>( maxStraight );
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
		trace( 'findStraightPath st=${recast.DetourStatus.fromInt( st )} count=$straightCount' );

		final out : Array<Point> = [];
		out.resize( straightCount );
		for ( i in 0...straightCount ) {

			final rx = straight[i * 3 + 0];
			final ry = straight[i * 3 + 1];
			final rz = straight[i * 3 + 2];

			out.push( new Point( rx, -rz, ry ) );
		}

		return out;
	}
}
