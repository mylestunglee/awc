PACKAGES="make clang-format clang-tidy cmake doxygen g++ gcc graphviz lcov libglpk-dev libgtest-dev libxml2-utils"

if (apt-cache policy $PACKAGES | grep "Installed: (none)"); then
    sudo apt install make $PACKAGES
fi

if [ ! -f /usr/lib/libgtest.a ] || [ ! -f /usr/lib/libgtest_main.a ]; then
    (cd /usr/src/gtest && sudo cmake CMakeLists.txt && sudo make $@ && sudo cp ./lib/libgtest*.a /usr/lib)
fi

make && ./src/awc maps/war_room_spann_island
