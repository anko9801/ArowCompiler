; ModuleID = 'bench.arow'
source_filename = "bench.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @print(i32)

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
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %var_tmp1 = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp1, 100000000
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp2 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp2, 1
  store i32 %add_tmp, i32* %i
  %var_tmp3 = load i32, i32* %i
  %call_tmp = call i1 @printnum(i32 %var_tmp3)
  %var_tmp4 = load i32, i32* %i
  %var_tmp5 = load i32, i32* %i
  %ifcond6 = icmp slt i32 %var_tmp5, 100000000
  br i1 %ifcond6, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i32 0
}
