; ModuleID = 'test5.arow'
source_filename = "test5.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %k = alloca i1
  store i1 false, i1* %k
  %a = alloca i32
  store i32 32, i32* %a
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
