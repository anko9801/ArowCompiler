; ModuleID = 'printnum.arow'
source_filename = "printnum.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %call_tmp = call i1 @"\01_printnum"(i32 1)
  %call_tmp1 = call i1 @"\01_printnum"(i32 3)
  %call_tmp2 = call i1 @"\01_printnum"(i32 3)
  ret i32 0
}
