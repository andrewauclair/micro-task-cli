Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.2
)

set(SOCKPP_BUILD_STATIC ON)
set(SOCKPP_BUILD_SHARED OFF)
FetchContent_Declare(
        sockpp
        GIT_REPOSITORY https://github.com/fpagliughi/sockpp.git
        GIT_TAG        v0.8.1
)

FetchContent_MakeAvailable(Catch2 sockpp)