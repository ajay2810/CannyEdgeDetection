# canny_edge_my_face
Exercise 2- Canny Edge Detector - ROS Package

As observed, I have chosen C++ in order to code this exercise. It works with two nodes: The first one, custom_cv_camera, is a simple 
node created by myself which mainly captures the frames from a webcam, save them on a OpenCV Mat variable and transform them to a 
ROS sensors_msgs/Image message. The second node, canny_edge, is the main node where the image processing is done. The node subscribes 
the Image messages from the custom_cv_camera, transform them to a OpenCV variable and process them. The resulting images are transformed 
again to a ROS Image message in order to be published and used by other nodes or GUI visualization tools. The threshold which indicates
in the Canny algorithm if a pixel is rejected can be modified with a rqt_reconfigure GUI.

Some remarks about this project
1.- In order to transform from OpenCV to ROS image and the other way around, should be installed the vision_opencv libraries. In
    case they are not installed, you can download and unzip in your workspace/src and then compile, the following repository:
    https://github.com/ros-perception/vision_opencv.git
    
2.- From the previous remark, some of the vision_opencv libraries needed for the project only run with OpenCV 2.x.x. If you have
    a newer version (version 3.x) then you must uninstall it and install the old one, or you can have both and modify CMakeLists.txt
    in order to depend only on the OpenCV 2 libraries.
    
3.- The Canny edge dectector algorithm is the one showed by the OpenCV documentation tutorials.

Steps to build and run:- 
1) Create a catkin workspace and clone/copy this package and vision_opencv from the above given link in the src folder. Example:-  "catkin_ws/src"
2) Build the package from the root folder of catkin workspace with the command "catkin_make".
3) Source the setup.bash files generated as needed for every terminal by ROS.
4) Launch the launch file to see the result :- roslaunch canny_edge_my_face canny_edge.launch

You can now the raw_image and the filterd image showing the canny_edges. You can also configure the Threshold value of the canny_edge detection algorithm by rqt_reconfigure GUI window using the slider.

Code Quality Tools used:- 
1) roslint for linting of the code.
2) cppcheck for syntex errors.

Refrences:-
http://wiki.ros.org/cv_camera
https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
http://wiki.ros.org/dynamic_reconfigure/Tutorials/SettingUpDynamicReconfigureForANode

