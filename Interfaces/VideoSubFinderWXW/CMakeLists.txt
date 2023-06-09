file(GLOB VideoSubFinderWXW_src
     "*.h"
     "*.cpp"
)

if (WIN32)
	add_executable(VideoSubFinderWXW WIN32 ${VideoSubFinderWXW_src} VideoSubFinderWXW.rc)
else()
	add_executable(VideoSubFinderWXW ${VideoSubFinderWXW_src})
endif (WIN32)

target_compile_features(VideoSubFinderWXW PUBLIC cxx_std_17)

target_include_directories(VideoSubFinderWXW PUBLIC
	${wxWidgets_INCLUDE_DIRS}
	 ${OpenCV_INCLUDE_DIRS}
	"../../Components/Include"
	"../../Components/IPAlgorithms"
	"../../Components/OCVVideo"
	"../../Components/FFMPEGVideo"
)

if (WIN32)
	target_link_directories(VideoSubFinderWXW  PUBLIC
		${VideoSubFinderWXW_LINK_DIRS}		
	)

	target_link_libraries(VideoSubFinderWXW PUBLIC
		IPAlgorithms
		OCVVideo
		${CUDAKernels_LIB}
		wxmsw32u${D}_aui.lib
		wxmsw32u${D}_media.lib
		wxmsw32u${D}_core.lib
		wxmsw32u${D}_adv.lib
		wxbase32u${D}.lib
		wxtiff${D}.lib
		wxjpeg${D}.lib
		wxpng${D}.lib
		wxzlib${D}.lib
		wxregexu${D}.lib
		wxexpat${D}.lib
		winmm.lib
		comctl32.lib
		rpcrt4.lib
		wsock32.lib
		odbc32.lib
		vfw32.lib
		${OpenCV_LIBS}
		libcrypto.lib
		libcurl.lib
		jsoncpp.lib
	)	
else()
	target_compile_definitions(VideoSubFinderWXW PUBLIC
		${wxWidgets_DEFINITIONS}
		$<$<CONFIG:Debug>:${wxWidgets_DEFINITIONS_DEBUG}>)
	target_compile_options(VideoSubFinderWXW PRIVATE ${wxWidgets_CXX_FLAGS})

	target_link_directories(VideoSubFinderWXW  PUBLIC
		${wxWidgets_LIBRARY_DIRS}
		${CUDA_LINK_DIRS}
	)

	target_link_libraries(VideoSubFinderWXW PUBLIC
		IPAlgorithms
		OCVVideo
		${CUDAKernels_LIB}
		${wxWidgets_LIBRARIES}
		${OpenCV_LIBS}
		tbb
		X11
		crypto
		curl
		jsoncpp
	)
endif (WIN32)


