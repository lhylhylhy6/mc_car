import sensor, image, time, pyb
from pyb import UART
import json
from time import sleep
# For color tracking to work really well you should ideally be in a very, very,
# very, controlled enviroment where the lighting is constant...
red_threshold   = (27, 49, 17, 127, -98, 127)
# You may need to tweak the above settings for tracking green things...
# Select an area in the Framebuffer to copy the color settings.
pyb.LED(1).on()
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # use RGB565.
sensor.set_framesize(sensor.QVGA) # use QQVGA for speed.
sensor.skip_frames(10) # Let new settings take affect.
sensor.set_auto_whitebal(False) # turn this off.
time.sleep_ms(500)
pyb.LED(1).off()
#pyb.LED(2).on()
#pyb.LED(3).on()
clock = time.clock() # Tracks FPS.
uart = UART(3, 115200)
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob=blob
            max_size = blob.pixels()
    return max_blob
i=0
flag=0
while(True):
    #img = sensor.snapshot() # Take a picture and return the image.
    stright = 0
    left=0
    right=0
    mid=0
    img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
    blobs1 = img.find_blobs([red_threshold],roi=(100,210,100,80))
    blobs2 = img.find_blobs([red_threshold],roi=(5,165,35,35))
    blobs3 = img.find_blobs([red_threshold],roi=(280,165,35,25))
    if(flag) :
        i = i+1
        if  i==25 :
               i = 0
               flag = 0
    if blobs1:
        max_blobs1=find_max(blobs1)
        img.draw_rectangle(max_blobs1.rect())
        img.draw_cross(max_blobs1.cx(), max_blobs1.cy())
        stright = 1
        mid=1
        x=max_blobs1.x()+max_blobs1.w()/2
        output_str="[%d]" % (x)
    if blobs2:
        max_blobs2=find_max(blobs2)
        img.draw_rectangle(max_blobs2.rect())
        img.draw_cross(max_blobs2.cx(), max_blobs2.cy())
        left = 1
    elif blobs3:
        max_blobs3=find_max(blobs3)
        img.draw_rectangle(max_blobs3.rect())
        img.draw_cross(max_blobs3.cx(), max_blobs3.cy())
        right = 1

    if (mid & (left or right) & (i==0)):
        print('c')
        output_str="[c]"
        print('you send:',output_str)
        uart.write(output_str+'\r\n')
        output_str="[m]"
        flag=1
    elif stright:

        uart.write(output_str+'\r\n')
        time.sleep_ms(10)

