; ModuleID = 'float.arow'
source_filename = "float.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define float @main() {
entry:
  %add_tmp = fadd float 3.000000e+00, 4.000000e+00
  %0 = fptosi float %add_tmp to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %sub_tmp = fsub float 3.000000e+00, 4.000000e+00
  %1 = fptosi float %sub_tmp to i32
  %call_tmp4 = call i1 @printnum(i32 %1)
  %mul_tmp = fmul float 3.000000e+00, 4.000000e+00
  %2 = fptosi float %mul_tmp to i32
  %call_tmp7 = call i1 @printnum(i32 %2)
  %div_tmp = fdiv float 3.000000e+00, 4.000000e+00
  %3 = fptosi float %div_tmp to i32
  %call_tmp10 = call i1 @printnum(i32 %3)
  %add_tmp13 = fadd float 3.000000e+00, 4.000000e+00
  ret float %add_tmp13
}
