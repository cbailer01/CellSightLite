add_library(cellsight_core STATIC
    src/Cell.cpp
    src/CycleAnalyzer.cpp
    src/DataImporter.cpp
    src/Datasheet.cpp
    src/Estimators.cpp
    src/Exporter.cpp
    src/Integration.cpp
    src/Metrics.cpp
    src/Repository.cpp
    src/TestBench.cpp
    src/TestPlan.cpp
    src/TimeSeries.cpp
    src/User.cpp
)
target_include_directories(cellsight_core PUBLIC include)
