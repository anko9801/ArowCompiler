; ModuleID = 'diffint.arow'
source_filename = "diffint.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define float @main() {
entry:
  %0 = fpext half 0xH4600 to float
  ret float %0
}
