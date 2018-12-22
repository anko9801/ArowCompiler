; ModuleID = 'test7.arow'
source_filename = "test7.arow"

declare i32 @printnum(i32)

declare i32 @sleep(i32)

declare i32 @usclock()

declare i32 @BlinkLED(i32)

declare i32 @GPIOsetup()

declare i32 @GPIOclear()

define i32 @main() {
entry:
  ret i32 0
}
