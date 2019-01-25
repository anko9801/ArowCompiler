; ModuleID = 'float.arow'
source_filename = "float.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i1 @sleep(i32 %i_arg) {
entry:
  %i = alloca i32
  store i32 %i_arg, i32* %i
  %var_tmp = load i32, i32* %i
  %mul_tmp = mul i32 %var_tmp, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define float @main() {
entry:
  %a = alloca float
  store float 3.000000e+00, float* %a
  %b = alloca float
  store float 4.000000e+00, float* %b
  %var_tmp = load float, float* %a
  %var_tmp1 = load float, float* %b
  %add_tmp = fadd float %var_tmp, %var_tmp1
  %0 = fptosi float %add_tmp to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %var_tmp2 = load float, float* %a
  %var_tmp3 = load float, float* %b
  %sub_tmp = fsub float %var_tmp2, %var_tmp3
  %1 = fptosi float %sub_tmp to i32
  %call_tmp4 = call i1 @printnum(i32 %1)
  %var_tmp5 = load float, float* %a
  %var_tmp6 = load float, float* %b
  %mul_tmp = fmul float %var_tmp5, %var_tmp6
  %2 = fptosi float %mul_tmp to i32
  %call_tmp7 = call i1 @printnum(i32 %2)
  %var_tmp8 = load float, float* %a
  %var_tmp9 = load float, float* %b
  %div_tmp = fdiv float %var_tmp8, %var_tmp9
  %3 = fptosi float %div_tmp to i32
  %call_tmp10 = call i1 @printnum(i32 %3)
  %var_tmp11 = load float, float* %a
  %var_tmp12 = load float, float* %b
  %add_tmp13 = fadd float %var_tmp11, %var_tmp12
  ret float %add_tmp13
}
