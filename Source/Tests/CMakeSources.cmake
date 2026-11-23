set (TE_TESTS_INC_NOFILTER
)

set (TE_TESTS_SRC_NOFILTER
    "AudioUtility.test.cpp"
    "Renderer.test.cpp"
    "String.test.cpp"
)

source_group ("" FILES ${TE_TESTS_SRC_NOFILTER} ${TE_TESTS_INC_NOFILTER})

set (TE_TESTS_SRC
    ${TE_TESTS_INC_NOFILTER}
    ${TE_TESTS_SRC_NOFILTER}
)
