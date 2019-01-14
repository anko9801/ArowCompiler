; ModuleID = 'if3.arow'
source_filename = "if3.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %ifcond = icmp eq i32 %var_tmp, 5
  %var_tmp1 = load i32, i32* %i
  %ifcond2 = icmp sgt i32 %var_tmp1, -1
  %and_cond = and i1 %ifcond, %ifcond2
  br i1 %and_cond, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp3 = load i32, i32* %i
  %call_tmp = call i1 @printnum(i32 %var_tmp3)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
