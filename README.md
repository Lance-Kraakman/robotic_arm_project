# espIDF robotic arm project.

## see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#component-directories for description of the project layout

### Project is in progress!!!
### I am learning c++ but have experience with OOP and lower level programming levels (Python, C, Java etc) so any advice or help would be apreciated if you have any

*********						Application Notes						**********

Cannot log/print to console in doTrajectory method and the required methods and functions for do trajectory as the hardware tx buffer will overflow causeing a delay causing drivers to not function proprely.
No fix at the moment because even if we print one char every iteration the UART cannot transmit the data fast enough. (Without there being an overflow in the hardware buffer and therefore a blocking UART transmit call)

We can run the motors at the same time because we have two cores and a Symmetric multiprocessing (SMP) archetecture. This allows us to have one peice of code in a critical section and run another task at the same time. Because of this we can drive our stepper motors and transfer data at the same time.

(For more about this read below link)
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/freertos-smp.html#critical-sections-disabling-interrupts