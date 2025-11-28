package recast;

import haxe.extern.EitherType;

enum abstract StatusType( String ) {

	var SUCCESS;
	var FAILURE;

	var IN_PROGRESS;
	var WRONG_MAGIC;
	var WRONG_VERSION;
	var OUT_OF_MEMORY;
	var INVALID_PARAM;
	var BUFFER_TOO_SMALL;
	var OUT_OF_NODES;
	var PARTIAL_RESULT;
	var ALREADY_OCCUPIED;
}

class DetourStatus {

	static inline var DT_FAILURE = 1 << 31; // 0x80000000
	static inline var DT_SUCCESS = 1 << 30; // 0x40000000
	static inline var DT_IN_PROGRESS = 1 << 29; // 0x20000000
	static inline var DT_DETAIL_MASK = 0x0FFFFFFF;

	static inline var DT_WRONG_MAGIC = 1 << 0;
	static inline var DT_WRONG_VERSION = 1 << 1;
	static inline var DT_OUT_OF_MEMORY = 1 << 2;
	static inline var DT_INVALID_PARAM = 1 << 3;
	static inline var DT_BUFFER_TOO_SMALL = 1 << 4;
	static inline var DT_OUT_OF_NODES = 1 << 5;
	static inline var DT_PARTIAL_RESULT = 1 << 6;
	static inline var DT_ALREADY_OCCUPIED = 1 << 7;

	public static function fromInt( dtStatus : Int ) : Array<StatusType> {

		if ( failed( dtStatus ) )
			return [FAILURE];

		if ( succeed( dtStatus ) )
			return [SUCCESS];

		if ( inProgress( dtStatus ) )
			return [IN_PROGRESS];

		final d = detail( dtStatus );
		final out = [];

		if (( d & DT_WRONG_MAGIC ) != 0 )
			out.push( WRONG_MAGIC );

		if (( d & DT_WRONG_VERSION ) != 0 )
			out.push( WRONG_VERSION );

		if (( d & DT_OUT_OF_MEMORY ) != 0 )
			out.push( OUT_OF_MEMORY );

		if (( d & DT_INVALID_PARAM ) != 0 )
			out.push( INVALID_PARAM );

		if (( d & DT_BUFFER_TOO_SMALL ) != 0 )
			out.push( BUFFER_TOO_SMALL );

		if (( d & DT_OUT_OF_NODES ) != 0 )
			out.push( OUT_OF_NODES );

		if (( d & DT_PARTIAL_RESULT ) != 0 )
			out.push( PARTIAL_RESULT );

		if (( d & DT_ALREADY_OCCUPIED ) != 0 )
			out.push( ALREADY_OCCUPIED );

		return out;
	}

	public static inline function failed( s : Int ) : Bool
		return( s & DT_FAILURE ) != 0;

	public static inline function succeed( s : Int ) : Bool
		return( s & DT_SUCCESS ) != 0;

	public static inline function inProgress( s : Int ) : Bool
		return( s & DT_IN_PROGRESS ) != 0;

	public static inline function detail( s : Int ) : Int
		return s & DT_DETAIL_MASK;
}
