; ModuleID = 'cast.arow'
source_filename = "cast.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %0 = zext i16 2 to i32
  ret i32 %0
}
