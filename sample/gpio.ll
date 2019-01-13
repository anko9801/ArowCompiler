; ModuleID = 'gpio.arow'
source_filename = "gpio.arow"

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i8 @gpioRead(i32)

declare i1 @gpioClear(i32)
