# nebula-eye
This is an interactive disposal based on Raspberry Pi camera and Max patches to detect people activity in the area

On the Raspberry Pi, start the stream with : 

    raspivid -t 0 -b 2000000 -fps 60 -w 640 -h 480 -sa -100 -o - | gst-launch-1.0 -e -vvv fdsrc ! h264parse ! rtph264pay pt=96 config-interval=5 ! udpsink host=<DESTINATION_IP> port=5000

And don't forget to add the destination computer IP.

You also play the `movie.mov` file inside the `bin` folder.

All the parameters are saved together in one `settings.xml` file and recalled automatically on start.
Background subtraction and motion flow algorithms will run on the OpenCL device if available.

# Dependencies

This is an openFrameworks application, so you need openFrameworks to build it : http://openframeworks.cc/download/
The common way is to put this repo inside the `apps/myApps` folder.
Then use the openFrameworks project generator to generate an IDE project.

The alternative way, is to use a CMake build system : https://github.com/aspeteRakete/of.
If you feel confortable with CMake but doesn't know openFrameworks very well, this could be easier.
Also, with CMake you choose your project generator, and then you can use Ninja.

## Addons

This project depends on severals OF addons.
There are setted as git submodules, but it seems that openFrameworks project generator assumes all addons should be in the main addons repository.
Which is bad to my eyes. I think it's better to use a specific commit of each addon then you can have different version of the same addon accross projects,
and the update of such an addon doesn't break your build. But this is only my point of view.
So if you're using the OF project generator, you will have to move all the `ofx*` folders to of's addons folder.
Check addons.make and / or CMakeLists' `ofxaddon()` calls to see which one are needed.

CMake build system support these local addon out-of-the-box.
So after update the submodule you be ready to configure and build.

### Known issues on OSX

CMake system fails to build some ofxOSC dependency
and the OpenCV library shipped with openFraworks doesn't include openCL support (which is needed).

You have to install opencv library from Homebrew :
    brew install opencv

Then generate an XCode project from openFrameworks "Project Generator" and patch the XCode project to add opencv header path and link libraries.
A preconfigured XCode project is included in this repository.
