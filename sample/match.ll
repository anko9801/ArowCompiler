; ModuleID = 'match.arow'
source_filename = "match.arow"

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
  store i32 2, i32* %a
  br label %ifcont4

else:                                             ; preds = %entry
  br i1 true, label %then1, label %else3

then1:                                            ; preds = %else
  %var_tmp2 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp2)
  br label %ifcont

else3:                                            ; preds = %else
  br label %ifcont

ifcont:                                           ; preds = %else3, %then1
  br label %ifcont4

ifcont4:                                          ; preds = %ifcont, %then
  %var_tmp5 = load i32, i32* %a
  %call_tmp6 = call i1 @printnum(i32 %var_tmp5)
  ret i32 0
}
