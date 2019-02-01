; ModuleID = 'remainder.arow'
source_filename = "remainder.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i32 @getPID()

define i1 @sleep(i32 %i_arg) {
entry:
  %i = alloca i32
  store i32 %i_arg, i32* %i
  %var_tmp = load i32, i32* %i
  %mul_tmp = mul i32 %var_tmp, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %a = alloca i32
  store i32 3, i32* %a
  %b = alloca float
  store float 2.000000e+00, float* %b
  %var_tmp = load i32, i32* %a
  %0 = sitofp i32 %var_tmp to float
  %var_tmp1 = load float, float* %b
  %rem_tmp = frem float %0, %var_tmp1
  %1 = fptosi float %rem_tmp to i32
  %call_tmp = call i1 @printnum(i32 %1)
  %var_tmp2 = load i32, i32* %a
  %2 = sitofp i32 %var_tmp2 to float
  %var_tmp3 = load float, float* %b
  %rem_tmp4 = frem float %2, %var_tmp3
  %3 = fptosi float %rem_tmp4 to i32
  ret i32 %3
}
