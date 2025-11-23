package recast;

// #if !macro
// typedef Recast = haxe.macro.MacroType<[webidl.Module.build(( { idlFile : "recast/recast.idl", autoGC : true, nativeLib : "recast", target : TargetHL, /* packageName : "recast",  */chopPrefix : "dt"/* , architecture : ArchX86_64  */} : idl.Options ) )]>;
// #else
#if !macro
typedef Recast = haxe.macro.MacroType<[idl.Module.build( {
	idlFile : "recast/recast.idl",
	chopPrefix : "dt",
	autoGC : true,
	nativeLib : "recast",
	packageName : "recast",
	target : "hl",
	architecture : "x86_64"
} )]>;
#else
class Recast {

	static var config : webidl.Options = {
		idlFile : "recast/recast.idl",
		autoGC : true,
		outputDir : "src",
		nativeLib : "recast",
	};

	public static function build() {
		// config.chopPrefix = "dt";
		return webidl.Module.build( config );
	}

	#if eval
	static var INCLUDE = '
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
    ';

	public static function buildLibCpp() {
		config.includeCode = INCLUDE;
		webidl.Generate.generateCpp( config ); // will emit src/recast.cpp
	}

	public static function getFiles() {
		config.includeCode = INCLUDE;
		var sources = sys.io.File.getContent( "sourcelist.txt" )
			.split( "\n" )
			.map( s -> StringTools.trim( s ) )
			.filter( s -> s.length > 0 );
		return sources;
	}

	// Optional: JS/wasm build, mirroring bullet’s ammo.js bits.
	// If you don’t care about JS, you can just delete this.
	public static function buildLibJS() {
		var debug = false;
		var totalMemory = 256 * 1024 * 1024;
		var defines = debug ? [] : [
			"NO_EXIT_RUNTIME=1",
			"NO_FILESYSTEM=1",
			"AGGRESSIVE_VARIABLE_ELIMINATION=1",
			"NO_DYNAMIC_EXECUTION=1",
			"TOTAL_MEMORY=" + totalMemory
		];

		var params = [
			"-O" + ( debug ? "0" : "3" ),
			"--llvm-lto", "1",
			"-I", "lib/recastnavigation/Detour/Include",
			"-I", "lib/recastnavigation/Recast/Include",
			"-I", "lib/recastnavigation/DetourCrowd/Include",
			"-I", "lib/recastnavigation/DetourTileCache/Include",
			"-I", "lib/recastnavigation/DebugUtils/Include"
		];

		for ( d in defines ) {
			params.push( "-s" );
			params.push( d );
		}

		config.outputDir = "build/emscripten";
		webidl.Generate.generateJs( config, getFiles(), params );
	}
	#end
}
#end
