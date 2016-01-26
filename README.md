# nebula-eye
This is an interactive disposal based on Raspberry Pi camera and Max patches to detect people activity in the area

On the Raspberry Pi, start the stream with : 

    raspivid -t 0 -b 2000000 -fps 60 -w 640 -h 480 -sa -100 -o - | gst-launch-1.0 -e -vvv fdsrc ! h264parse ! rtph264pay pt=96 config-interval=5 ! udpsink host=<DESTINATION_IP> port=5000

And don't forget to add the destination computer IP.
