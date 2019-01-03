; ModuleID = 'diffint.arow'
source_filename = "diffint.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define float @main() {
entry:
  %0 = fpext half 0xH4600 to float
  ret float %0
}
