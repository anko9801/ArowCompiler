; ModuleID = 'match2.arow'
source_filename = "match2.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i32
  store i32 5, i32* %a
  %var_tmp = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp, 5
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp1 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp1)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp2 = load i32, i32* %a
  ret i32 %var_tmp2
}
