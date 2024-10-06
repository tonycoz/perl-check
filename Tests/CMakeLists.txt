#configure_file("${CMake_SOURCE_DIR}/.clang-format" ".clang-format" COPYONLY)

function(add_run_clang_tidy_test check_name)
  add_test(NAME "RunClangTidy.${check_name}" COMMAND ${CMAKE_COMMAND}
    "-DCLANG_TIDY_COMMAND=$<TARGET_FILE:clang-tidy>"
    "-DCLANG_TIDY_MODULE=$<TARGET_FILE:perl-check>"
    "-DCHECK_NAME=${check_name}"
    "-DRunClangTidy_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/RunClangTidy.cmake"
    )
endfunction()

add_run_clang_tidy_test(perl-literal-sv_setpvn)
add_run_clang_tidy_test(perl-literal-newSVpvn)
add_run_clang_tidy_test(perl-literal-newSVpvn_flags)
add_run_clang_tidy_test(perl-literal-hv_fetch)

add_run_clang_tidy_test(perl-undef-sv_setsv)
