; ModuleID = 'iscast.arow'
source_filename = "iscast.arow"

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

define i32 @main() {
entry:
  %i = alloca i32
  store i32 4, i32* %i
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp = load i32, i32* %i
  %call_tmp = call i1 @printnum(i32 %var_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp1 = load i32, i32* %i
  ret i32 %var_tmp1
}
