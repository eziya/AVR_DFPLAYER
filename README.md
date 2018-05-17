# AVR_DFPLAYER
DFPlayer library for AVR<br>

This is an AVR library for DFPlayer. <br>
DFPlayer product link<br>
https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299<br><br>

I used a reference manual from Flyron Technology Co., Ltd<br>
(http://www.flyrontech.com/edownload/6.html)<br>

Hardware UART was used to communicate with DFPlayer and software UART to communicate with PC as Atmega328p has only one UART port. Software UART codes were written by danni and I modified some codes for better understanding and coding style.
(https://community.atmel.com/projects/second-uart-software)

This library was only tested with Atmega328p so may need to change hardware and software UART codes<br>
if you're using diffent MCU from AVR family.

I tried to implement all of features but some of optional features are not implemented such as feedback.<br> 
Below link is testing video<br>
https://youtu.be/GS12IwCR5NU
