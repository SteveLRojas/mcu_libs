# mcu_libs
Place to keep all my libraries for various MCUs

## Things worthy of note
Some of the supported MCUs have bugs or deficiencies that might make them unsuitable for certain projects. Below are some of the most severe issues I have found so far.  

### CH552
- The analog comparator cannot generate interrupts, it has to be polled.  
- The USB controller has a bug that prevents interrupt transfers from working when SOF interrupts are enabled.  
- Low write endurance of about 200 cycles, and must be powered from 5V for programming.  
- When writing data flash interrupts must be manually disabled, otherwise very strange behaviour can occur.  
- When multiple GPIO interrupt pins are enabled there is no way to know which one triggered the interrupt.  

### CH559
- The analog comparator cannot generate interrupts, it has to be polled.  
- The ADC gives nonsense readings when sampling a low voltage.  
- The ADC is missing clock domain crossing for the trigger input. The trigger pulse has to be timed by the software.  
- The USB controller has a bug that prevents interrupt transfers from working when SOF interrupts are enabled.  
- Although there are 2 USB ports there is only one USB controller. It is not possible to have a USB host and a USB device simultaneously, and only one port can act as a USB device.  
- The USB controller does not indicate when a transaction is attempted (nor does it provide a timeout), it only indicates when a transaction completes. The software has to handle the timeout in case a transaction fails or receives no response.  
- The USB controller requires the enpoint buffers to be placed at even addresses (This can be annoying when adapting software from the CH552).  

### CH32V203
- Parts of the USB libraries provided by WCH are pre-compiled (no source available).  
- The USBD controller uses a 16-bit memory called PMA, this memory can only be accessed as 16-bit words (writing single bytes not possible) and the odd numbered words are not implemented (these need to be skipped when reading or writing sequentially). The PMA is also slower than the main memory which makes using the USBD module completely annoying.  
- The EPR registers in the USBD module have "write 1 to toggle" bits as well as "write 0 to clear" and "read write" bits. This makes accessing the EPR register very tedious and error prone.  
- The SOF interrupt feature has been removed from the documentation for the USBFS controller (the feature is still there in the hardware and still bugged).  
- The USBFS module gives wrong endpoint number for setup tokens. It may not be possible to have multiple control endpoints.  
