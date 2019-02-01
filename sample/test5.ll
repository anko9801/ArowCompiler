; ModuleID = 'test5.arow'
source_filename = "test5.arow"

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
  %k = alloca i1
  store i1 false, i1* %k
  %a = alloca i32
  store i32 32, i32* %a
  br i1 false, label %then, label %else4

then:                                             ; preds = %entry
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp ne i32 %var_tmp1, 0
  br i1 %ifcond, label %then2, label %else

then2:                                            ; preds = %then
  br label %ifcont

else:                                             ; preds = %then
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then2
  %var_tmp3 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp3)
  br label %ifcont5

else4:                                            ; preds = %entry
  br label %ifcont5

ifcont5:                                          ; preds = %else4, %ifcont
  ret i32 0
}
