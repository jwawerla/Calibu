# Calibu #
======

## Dependencies: ##

### System Dependencies: ###

```
sudo apt-get install libgflags-dev libsuitesparse-dev libgoogle-glog-dev libatlas-base-dev libeigen3-dev libprotobuf-dev
```

### Other Dependencies ###

Calibu requires libraries for which apt-get does not provide binaries. CMakes external project utility can be used to build these dependencies.

## Build ##

Building Calibu is a bit tricky because of a circular dependency. HAL requires Calibu and calibgrid-hal requires HAL. The solutions is a two step build process:

First build all external dependencies except HAL
```
cd Calibu
cd external
cmake .
make
```

Next we build Calibu without HAL support
```
cd ..
mkdir build
cd build
cmake ..
make
```

Now we can go back to the dependencies and build HAL 
```
cd ..
cd external
rm CMakeCache.txt
cmake .
make
```

Finally we can build Calibu with HAL support
```
cd ..
cd build
make
```

## Run application ##

### With dc1394 cameras: ###

```
calibgrid-hal dc1394:[fmt=FORMAT7_0,size=640x480,iso=400,dma=10]//[0,1]
```

### From image files: ###

```
calibgrid-hal files:///images/[left,right]*.jpg
```

### Operation: ###

Once the application is started, make sure "Add Frames" is checked. Grab images for a while (~20-30 sec). Then unckeck "Add Frames" and press '[' to start the optimization. Observe the console output and once you see "FUNCTION TOLERANCE" the optimization has converged. Now you can terminate the application by pressing 'ECS'. The camera calibration is in cameras.xml.



