; ModuleID = 'excellent_type.arow'
source_filename = "excellent_type.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %call_tmp = call i1 @printnum(i32 0)
  ret i32 0
}
