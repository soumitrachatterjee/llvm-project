add_llvm_library(our_fdump_auto_inference MODULE our_fdump_auto_inference.cpp PLUGIN_TOOL clang)
if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(our_fdump_auto_inference PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()
