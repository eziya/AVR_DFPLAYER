# AVR_DFPLAYER
DFPlayer library for AVR<br>

This is a library code for DFPlayer. <br>
DFPlayer product link : https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299<br><br>

I used reference manual from Flyron Technology Co., Ltd<br>
(http://www.flyrontech.com/edownload/6.html)<br>

I used H/W USART to communicate with DFPlayer and S/W USART to communicate with PC.<br>
I modified S/W USART codes written by danni to implement S/W USART functions.
(https://community.atmel.com/projects/second-uart-software)

This library was tested with Atmega328p MCU only.<br>
You may need to change H/W USART codes if you use diffent MCU of AVR family.

Some of features are not implemented such as feedback command.<br> 
Library testing video<br>
https://youtu.be/GS12IwCR5NU



