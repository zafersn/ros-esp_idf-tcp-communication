# ros-esp_idf-tcp-communication
This directory contains simple examples of the BSD TCP Server Socket API for ESP-IDF and the combination of the ros turtle node and the tcp client socket apis. keyboard key data received with turtlesim_turtle_teleop_key is transmitted to esp32 on Tcp protocol and led blink application.

# --- ESP-IDF ---

 * Make esp-idf project **make menuconfig** or open **sdkconfig.h** file and type the ssd and password of the network to connect to the same network as ros.
 


## --- ROS ---
 * In the TCP_IP field in the testTcpClient.py file, type the ip and port address of the esp32 tcp server.
 * Let's run a node to start uart communication with esp on ubuntu and subscribe to / turtle1 / cmd_vel.
   - rosrun esp-idf-test testTcpClient.py
 * then subscribe and read the keys on the keyboard will start the node.
   - rosrun turtlesim turtle_teleop_key 
