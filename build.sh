if [[ "$1" ]]; then
	buildType="$1"
else
	read -p "Build type: " buildType
fi
mkdir -p bin
if [[ "${buildType^}" = "Web" ]]; then
	if [[ "$2" ]]; then
		buildType="$2"
	else
		buildType="Release"
		# read -p "Debug level: " buildType
	fi

	if [ "${buildType^}" != "Debug" ] && [ "${buildType^}" != "Release" ]; then
		buildType="Debug"
	fi
	mkdir -p build.web
	# cd build.web
	emcmake.py cmake -S . -B ./build.web -G "Ninja" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE="${buildType^}" -DPLATFORM=Web
	emmake.py cmake --build ./build.web
	exit
elif [ "${buildType^}" != "Debug" ] && [ "${buildType^}" != "Release" ]; then
	buildType="Debug"
fi
mkdir -p build
# cmake -DCMAKE_BUILD_TYPE="${buildType^}" .. -G "Unix Makefiles"
# cmake -DNFD_PORTAL=ON -DCMAKE_BUILD_TYPE="${buildType^}" .. -G "Unix Makefiles"
cmake -S . -B ./build -G "Ninja" -DNFD_PORTAL=ON -DCMAKE_BUILD_TYPE="${buildType^}"
cmake --build ./build
# make
# cd ..
if [[ "${buildType^}" = "Debug" ]]; then
	cd bin
	wezterm start --cwd . --always-new-process --class floating gdb -ex run ./$(basename $(dirname $PWD))
	# wezterm start --cwd . --always-new-process --class floating ./$(basename $(dirname $PWD))
fi
exit
