; ModuleID = 'remainder.arow'
source_filename = "remainder.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %0 = sitofp i32 3 to float
  %rem_tmp = frem float %0, 2.000000e+00
  %1 = fptosi float %rem_tmp to i32
  %call_tmp = call i1 @printnum(i32 %1)
  %2 = sitofp i32 3 to float
  %rem_tmp4 = frem float %2, 2.000000e+00
  %3 = fptosi float %rem_tmp4 to i32
  ret i32 %3
}
