add_executable(cellsight_tests
    main.cpp
    test_etape1.cpp
    test_etape2.cpp
    test_etape3.cpp
    test_etape4.cpp
    test_etape5.cpp
    test_etape6.cpp
    test_etape7.cpp
    test_bonus.cpp
)
target_link_libraries(cellsight_tests PRIVATE cellsight_core)
target_compile_definitions(cellsight_tests PRIVATE CELLSIGHT_DATA_DIR="${CELLSIGHT_DATA_DIR}")

# ctest -R etape3   : uniquement les tests de l'etape 3
foreach(n 1 2 3 4 5 6 7 9)
    add_test(NAME etape${n} COMMAND cellsight_tests --etape ${n})
endforeach()
