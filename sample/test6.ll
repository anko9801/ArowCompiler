; ModuleID = 'test6.arow'
source_filename = "test6.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp ne i32 4, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %ifcond2 = icmp ne i32 5, 0
  br i1 %ifcond2, label %then3, label %else4

then3:                                            ; preds = %ifcont
  br label %ifcont5

else4:                                            ; preds = %ifcont
  ret i32 0
  br label %ifcont5

ifcont5:                                          ; preds = %else4, %then3
  br i1 false, label %then6, label %else32

then6:                                            ; preds = %ifcont5
  %add_tmp = add i32 4, 5
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %ifcond10 = icmp ne i32 4, 0
  br i1 %ifcond10, label %then11, label %else12

then11:                                           ; preds = %then6
  br label %ifcont13

else12:                                           ; preds = %then6
  ret i32 0
  br label %ifcont13

ifcont13:                                         ; preds = %else12, %then11
  %ifcond15 = icmp ne i32 5, 0
  br i1 %ifcond15, label %then16, label %else17

then16:                                           ; preds = %ifcont13
  br label %ifcont18

else17:                                           ; preds = %ifcont13
  ret i32 0
  br label %ifcont18

ifcont18:                                         ; preds = %else17, %then16
  %sub_tmp = sub i32 4, 5
  %call_tmp21 = call i1 @printnum(i32 %sub_tmp)
  %ifcond23 = icmp ne i32 4, 0
  br i1 %ifcond23, label %then24, label %else25

then24:                                           ; preds = %ifcont18
  br label %ifcont26

else25:                                           ; preds = %ifcont18
  ret i32 0
  br label %ifcont26

ifcont26:                                         ; preds = %else25, %then24
  %ifcond28 = icmp ne i32 5, 0
  br i1 %ifcond28, label %then29, label %else30

then29:                                           ; preds = %ifcont26
  br label %ifcont31

else30:                                           ; preds = %ifcont26
  ret i32 0
  br label %ifcont31

ifcont31:                                         ; preds = %else30, %then29
  br label %ifcont41

else32:                                           ; preds = %ifcont5
  %ak = alloca i32
  store i32 4, i32* %ak
  %bk = alloca i32
  store i32 5, i32* %bk
  %var_tmp35 = load i32, i32* %ak
  %var_tmp36 = load i32, i32* %bk
  %mul_tmp = mul i32 %var_tmp35, %var_tmp36
  %call_tmp37 = call i1 @printnum(i32 %mul_tmp)
  %var_tmp38 = load i32, i32* %ak
  %var_tmp39 = load i32, i32* %bk
  %div_tmp = sdiv i32 %var_tmp38, %var_tmp39
  %call_tmp40 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont41

ifcont41:                                         ; preds = %else32, %ifcont31
  ret i32 0
}
