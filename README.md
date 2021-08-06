# 6DOF MOTION SIMULATOR TESTING

I'm working on building a 6 degree of freedom (6 DOF) Motion Simulator (also called a Stewart Platform or hexapod).  This Git repository is for some testing code and a short tutorial to help people doing the same thing get up to speed faster as I learn. There are lots of examples of similar projects. And some great examples of some full-scale truly DIY platforms including home-built actuators (e.g., [SilentChill](https://www.youtube.com/channel/UCNbwVIbv1c_SO7rM1etFW1g) and [Peacemaker Motion](https://www.youtube.com/watch?v=Ewiyuy38aO8)). But I found it hard to get started from ground zero as lots of the experts are (thankfully) actively developing software and hardware and not spending a ton of time making tutorials for newbies like me.

End goal: I plan on making something similar to the Silent Chill or Peacemaker Motion rig (see xsimulator.net).  It will probably not have screens since I'll use my Oculus.  I have a gaming PC with SteamVR and the Oculus Quest 2.  I just recently got Asetto Corsa and will use it as a source in the near future.  

Since the motion simulator hardware involves high power (lots of people use 240V input and I see designs using power supplies sized to consume up to 3KW at peak), are dangerous (actuators need to move people around quickly), and use somewhat expensive gear, I'm working on some desktop testing to tinker until I'm ready to commit to full scale.

For now this How-to is limited to the desktop sized test rig using open source software and shared models. 

------

# How-to make a desktop model to see how the hexapod works and see it move with FlyPT-Mover and your racing or flight simulator games.  
Following these steps should take about a workday (plus printing time).

[<img src="/img/front-image.png" width="400"/>](https://user-images.githubusercontent.com/6901860/128341217-a80dd36d-bdde-4e3c-92ce-daf63e1e8028.mp4)

(click picture to play video)

### Setting up Hardware:

**Servos**: Start with some cheap servos (need 6, this link is for 10): https://www.amazon.com/gp/product/B07L2SF3R4

**Model:** Make a 3D printed "Rig" ("rig is a FlyPT-Mover term). Here is a nice Thingiverse model for this: https://www.thingiverse.com/thing:2038246/files

**Servo Driver Board:** Get a PCA9685 (recommended, but not really required if you're using an ESP32). This makes servo hookup easy and this is what my code assumes.  This allows easy hookup of separate servo power supply. There are slightly cheaper ones, but this works well:  https://www.amazon.com/HiLetgo-PCA9685-Channel-12-Bit-Arduino/dp/B01D1D0CX2

**ESP32 Control Board:** Get a ESP32 dev board; this is nice since it is pretty fast, low cost, can directly do PWM from any input, and has integrated WiFi. Any brand of dev board should work, you'll just need to the know the board model for PlatformIO to configure itself for building and uploading.  I used this:  https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z/

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

![](/img/wiring.png)

2. Connect the ESP32 to your PC via USB.
3. Click "Monitor" in VSCODE (the plug icon at the bottom of VSCODE; this should reset the ESP32 and open a serial connection).

**Note:** When you connect this way it should also reconnect to WiFi and print out the IP Address of your device. *WRITE DOWN THE IP ADDRESS; YOU'LL NEED IT BELOW*. Otherwise go to your router to find the IP address for later use.

<u>**Test actuator range and and the servo arms:**</u>

1. In the Serial Monitor window type 0; then type 180. (You don't need to hit <enter>, just wait after typing). That should move the servo 0 degrees and then to 180 degrees.  **Note**: read the main.cpp code notes around SERVOMIN and SERVOMAX if adjustments are needed.
2. Then add the servo arm to each servo (assuming you left it at the 180 degree position - the servo arm should be pointing at whatever end is the bottom or "in/retracted").

------

### Build the 6DOF Desktop Rig:

1. Print all the parts for 3D rig from the Thingiverse page.  You'll need 6 of the actuator parts and 12 copies of the joint parts. I printed in PLA at 30% infill in the orientation downloaded with normal supports turned on.

2. Make sure to sand/smooth the sliding parts.  

3. Use a little superglue to put together the actuator. You can also just use scotch tape for testing (as demonstrated in my images). 

4. Attach the servos to the 6 actuators.

5. Use paper clips or other thin wire to connect servo arm to the sliding rod of the actuator.

6. Assemble the 6DOF rig printed model.
    a. I used M3 bolts and nuts to connect the ball-joints (which snap together) to the actuators.
    b. Then I used more M3 bolts and nuts to connect the 6 actuators to the top and bottom parts. 
    c. I ran the wires through the middle to the PCA2685.  (Servos are numbered 1-6 going clockwise starting from the one to the right of "front" when sitting behind the simulator rig.  See FlyPT-Mover images if that is confusing.  Connect the servos in order from channels 1-6 as shown.
    d. Note I am powering the servo-board separately from the ESP32 using a 5V power supply with a few amps capacity.  

   ![](/img/setup-top.JPG)
   ![](/img/setup-front.JPG)

------

### Setup FlyPT-Mover:

**Note**: FlyPT Mover basically interconnects motion sources, motion hardware, does calculations, and adds nice tools for tweaking, simulating, and visualizing. So in the steps below you'll add sources (just simulated motion loops for now); add a "pose" calculation; add a "rig" to tell it what your hardware looks like; add an "output" to send the actuator position commands (using UDP over WiFi for this test); and a 3D Viewer to see what actuator motion is being sent.

1. **Download FlyPT-Mover:** https://www.FlyPT-Mover.com/ 

2. Extract the files to a folder of your choice.

   ​	\>>TODO: Add a Git file with the saved settings for my MOVER files

3. **Run the executable.**

   You should see a blank interface:

   ![](/img/blank.png)

4. **Configure FlyPT-Mover:**

   ##### Add "Sources" for a simple sinusoid wave:*

   YAW SPEED

   1. Right click on the interface and select ">Add>Source>Loop"
   2. Check "Yaw speed" and uncheck all the other options on the right. Set up:

![](/img/yawspeed.png)

   ROLL POSITION

   1. Right click on the interface and select ">Add>Source>Loop"
   2. Check "Roll position" and uncheck all the other options on the right. Set up:

![](/img/rollposition.png)

   PITCH POSITION

   1. Right click on the interface and select ">Add>Source>Loop"
   2. Check "Pitch position" and uncheck all the other options on the right. Set up:

![](/img/pitchposition.png)


   ##### Add a "Pose" calculation from the motion sources:

   1. Right click on  the interface and select ">Add>Pose>From Motion"

   2. Check SOURCE::LOOP;  SOURCE::LOOP (1); SOURCE::LOOP (2) on right. (This links the sources to this pose calculation):

![](/img/pose.png)


   ##### Add a "Rig" to set up the hexapod:

   1. Right click on  the interface and select ">Add>Rig>From Motion"

![](/img/addrig.png)

   2. Edit the Rig "Position bit output" setting to 12 and check "POSE::FROM MOTION" for the source of motion.  

![](/img/editrig.png)

   3. Use the calculations spreadsheet (6DOF_Calulations_v2.xlsx) that is provided with the Thingiverse model (https://www.thingiverse.com/thing:2038246/files) to calculate the L1, L2, L3, L4 dimensions:

   \>>TODO: PROVIDE MY SETTINGS

   ##### Add an "Output" to  send your actuator positions to the ESP32 via UDP.

      1. Right click on the interface and select ">Add>Output>UDP"
      2. The port should match what you set in the code (set to 9777 by default).
      3. The IP address should match the ESP32 device (written down during test above).
      4. Type of Output should be "Decimal"
      5. The Output string should be "<Axis1a>;<Axis2a>;<Axis3a>;<Axis4a>;<Axis5a>;<Axis6a>;"  (semi-colons are needed to parse text in code):

![](/img/output.png)

   ##### Add a 3D viewer to watch the model move and make sure is working.

   1. Right click on the interface and select ">Add>Viewer>3D"); then select your Rig.

------

### Run an operational test:

1. Verify your software (main.cpp) is setup how you want:
    a. For normal operations you want the following flags set:
        #define USE_WIFI_AND_UDP_FOR_FLYPTMOVER true 
        #define ALLOW_SERIAL_INPUT false
        #define STREAM_SERIAL_OUTPUT false
        #define NUM_ACTUATORS 6
    b. If you didn't have these set, rebuild and upload you software to your ESP32. **Note:** It will work with serial I/O on, but will be slower and make the rig movement choppy.)

2. In FlyPT-Mover virtually "connect" the three loop sources (click the little square in upper left of each of the sources). They turn red when they are connected:

   ![](/img/connectoutput.png)

3. Look at the 3D Viewer and Rig (click the squares in upper right of those controls in FylPT to open them) to verify it is moving and the actuator position test is showing the calculated results and shifting left and right.

   ![](/img/seemotion.png)

4. Check or physically connect the ESP32/PCA9685/Rig-servos together and connect power (**Note:** You can power the ESP32 using its micro-USB input, but you should use a dedicated 5V power supply with at least a few amps to power the PCA9685 to avoid overloading the EPS32 regulators dep;ending on the servos you use).

5. Power on the desktop rig. When you power the ESP32 up if the #define ONBOARD_LED is set correctly for your board (usually 2) when it connects to WiFi it will flash 3 times. Then it is ready.

6. In FlyPT-Mover virtually "connect" the output (click the little square in upper left of output-UDP); it will turn red when on and the actuators should start moving.

   ![](/img/connectoutput.png)

7. Watch the rig follow the 3D viewer motion.

https://user-images.githubusercontent.com/6901860/128340775-fc95757a-c2b8-47d3-b3ef-ff1ae54d46d5.MOV

------

### Connect a game source:

1. Just like you added a loop source before, now add your game source. I am using Assetto Corsa Competizione here; but Flight Simulator, Epic Coaster Simulator, and others all work similarly.  Google for specific "start-up" instructions for each game.

   \>>TODO:: Add these instructions and images

