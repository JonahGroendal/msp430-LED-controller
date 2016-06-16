FILENAME = LEDController.c RGBController.c RGBPatturns.c

main: ${FILENAME}
	msp430-gcc -mmcu=msp430g2553 ${FILENAME} -o LEDController.elf
comp:	
	msp430-gcc -mmcu=msp430g2553 -o ${FILENAME} LEDController.elf -g
flash:	
	mspdebug rf2500 'prog LEDController.elf'

clean:
	rm -rf *.o *.elf *.out

erase:
	mspdebug rf2500 erase

debug: LEDController.elf
	(mspdebug rf2500 "gdb" 1>/dev/null &); msp430-gdb LEDController.elf -ex "target remote :2000"
