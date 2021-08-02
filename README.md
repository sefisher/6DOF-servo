# 6DOF MOTION SIMULATOR TESTING

Working on building a 6 degree of freedom (6 DOF) Motion Simulator (Stewart Platform); also called a hexapod.  This Git repository is for some testing code and a short tutorial to help people doing the same thing get up to speed faster as I learn.

## Making a desktop scaled model to test to see what all this does and wire everything together:

------

### Setting up Hardware:

**Servos**: Start with some cheap servos (need 6, this is 10): https://www.amazon.com/gp/product/B07L2SF3R4

**Model:** Make a 3D printed "Rig" ("rig is a FlyPT-Mover term). Here is a nice Thingiverse model for this: https://www.thingiverse.com/thing:2038246/files

**Servo Driver Board:** Get a PCA9685 (recommended, but not really required if you're using an ESP32). This makes servo hookup easy and this is what my code assumes.  This allows easy hookup of separate servo power supply. There are slightly cheaper ones, but this works well:  https://www.amazon.com/HiLetgo-PCA9685-Channel-12-Bit-Arduino/dp/B01D1D0CX2

**ESP32 Control Board:** Get a ESP32 dev board; this is nice since it is pretty fast, low cost, can directly do PWM from any input, and has integrated WiFi. Any brand of dev board should work, you'll just need to the know the board model for PlatformIO to configure itself for building and uploading.  I used this:  https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T

------

### Setting up Software:

Building and installing firmware software on ESP32 dev board:

 1. **<u>Install VSCODE</u>**: I use VSCODE with the PlatformIO extension installed.  Makes things easy.

2. **<u>Open VSCODE and run PlatformIO</u>** (click the PlatformIO icon [little alien head).

3. **<u>Clone this Git Project</u>**: In the PlatformIO quick access menu click ">Miscellaneous>Clone Git Project".  Use this link: https://github.com/sefisher/6DOF-servo for the project source.

4. **<u>Edit code Settings:</u>** Open the src\ folder and edit main.cpp:

      a. to add your wifi credentials (or make a new "secrets.h" file and add #define SSID "Your SSID" and #define WIFIPSWD "Your Password").

      b. Check the main.cpp code and ensure the #define ALLOW_SERIAL_INPUT macro is set to true.

5. **<u>Connect the ESP32</u>** (via USB cord).

6. **<u>Build/upload Firmware:</u>** Click Build (checkmark at bottom of VSCODE); then click Upload (arrow at bottom of VSCODE).

------

### Test/Setup Servo Actuators:
To test the actuator setup and ensure the calibration for your servos:

**<u>Connect:</u>**

1. Wire up the ESP32, PCA9685, and your servos without connecting them to the 6DOF Rig.

\>>TODO: Add pictures of wiring, 

2. Connect the ESP32 to your PC via USB.
3. Click "Monitor" in VSCODE (the plug icon at the bottom of VSCODE; this should reset the ESP32 and open a serial connection).

**Note:** When you connect this way it should also reconnect to WiFi and print out the IP Address of your device. *WRITE DOWN THE IP ADDRESS; YOU'LL NEED IT BELOW*. Otherwise go to your router to find the IP address for later use.

<u>**Test actuator range and and the servo arms:**</u>

1. In the Serial Monitor window type 0; then type 180. (You don't need to hit <enter>, just wait after typing). That should move the servo 0 degrees and then to 180 degrees.  **Note**: read the main.cpp code notes around SERVOMIN and SERVOMAX if adjustments are needed.
2. Then add the servo arm to each servo (assuming you left it at the 180 degree position - the servo arm should be pointing at whatever end is the bottom or "in/retracted").

------

### Build the 6DOF Desktop Rig:

1. Print all the parts for 3D rig from the Thingiverse page.  You'll need 6 of the actuator parts.

2. Make sure to sand/smooth the sliding parts.  

3. Use a little superglue to put together the actuator. You can just use scotch tape for testing. 

4. Attach the servos to the 6 actuators.

5. Use paper clips or other thin wire to connect servo arm to the sliding rod of the actuator.

6. Assemble the 6DOF rig printed model.

    \>>TODO: Add picture of servo actuator assembled, whole setup.

------

### Setup FlyPT-Mover:

**Note**: FlyPT Mover basically interconnects motion sources, motion hardware, does calculations, and adds nice tools for tweaking, simulating, and visualizing. So in the steps below you'll add sources (just simulated motion loops for now); add a "pose" calculation; add a "rig" to tell it what your     hardware looks like; add an "output" to send the actuator position commands (using UDP over WiFi for this test); and a 3D Viewer to see what actuator motion is being sent.

1. **Download FlyPT-Mover:** https://www.FlyPT-Mover.com/ 

2. Extract the files to a folder of your choice.

   ​	\>>TODO: Add a Git file with the saved settings for my MOVER files

3. **Run the executable.**

   You should see a blank interface:

   ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\blank.png)

   4. **Configure FlyPT-Mover:**

      

   ##### Add "Sources" for a simple sinusoid wave:*

   YAW SPEED

   1. Right click on the interface     and select ">Add>Source>Loop"
   2. Check "Yaw speed"     and uncheck all the other options on the right. Set up:

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\yawspeed.png)

   ROLL POSITION

   1. Right click on the interface     and select ">Add>Source>Loop"
   2. Check "Roll     position" and uncheck all the other options on the right. Set up:

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\rollposition.png)

   PITCH POSITION

   1. Right click on the interface     and select ">Add>Source>Loop"
   2. Check "Pitch     position" and uncheck all the other options on the right. Set up:

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\pitchposition.png)

   

   ##### Add a "Pose" calculation from the motion sources:

   1. i. Right click on      the interface and select ">Add>Pose>From Motion"

   2. ii. Check SOURCE::LOOP;      SOURCE::LOOP (1); SOURCE::LOOP (2) on right. (This links the sources to this pose calculation):

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\pose.png)

   

   ##### Add a "Rig" to set up the hexapod:

   1. 1. Right click on  the interface and select ">Add>Rig>From Motion"

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\addrig.png)

   

   ​		2. Edit the Rig "Position bit output" setting to 12 and check "POSE::FROM MOTION" for the source of motion.          

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\editrig.png)

   ​		3. Use the calculations spreadsheet (6DOF_Calulations_v2.xlsx) that is provided with the Thingiverse model (https://www.thingiverse.com/thing:2038246/files) to calculate the L1, L2, L3, L4 dimensions:

   \>>TODO: PROVIDE MY SETTINGS

   ##### Add an "Output" to  send your actuator positions to the ESP32 via UDP.

   1. 1. Right click on the interface and select ">Add>Output>UDP"
      2. The port should match the code (set to 8266 by default).
      3. The IP address should match the ESP32 device (written down during test above).
      4. Type of Output should be "Decimal"
      5. The Output string should be "<Axis1a>;<Axis2a>;<Axis3a>;<Axis4a>;<Axis5a>;<Axis6a>;"  (semi-colons are needed to parse text in code):

    ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\output.png)

   ##### Add a 3D viewer to watch the model move and make sure is working.

   ​		  1. Right click on the interface and select ">Add>Viewer>3D"); then select your Rig.


------

### Run the Test:

1. In FlyPT-Mover virtually "connect" the sources (click the little square in upper left of each of the sources). They turn red when they are connected:

   ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\connectoutput.png)

2. Look at the 3D Viewer and Rig (click the squares in upper right of those controls in FylPT to open them) to verify it is moving and the actuator position test is showing the calculated results and shifting left and right.

   ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\seemotion.png)

3. Check or physically connect the ESP32/PCA9685/Rig-servos to together and power them (**Note:** you should use a dedicated 5V power supply with at least a few amps to power the PCA9685 to avoid overloading the EPS32 regulators).

4. In FlyPT-Mover virtually "connect" the output (click the little square in upper left of output-UDP); it will turn red when on and the actuators should start moving.

   ![](D:\fishyDIYprojects\PlatformIO\Projects\6DOF servo\img\connectoutput.png)
