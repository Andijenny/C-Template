set(essential_src
    UnitEssentials.cpp
    main.cpp)

ADD_EXECUTABLE(UnitEssential ${essential_src})

TARGET_LINK_LIBRARIES(UnitEssential
    libgtest
    libgmock
)

add_test(NAME UnitEssential
         COMMAND UnitEssential)
