; ModuleID = 'match.arow'
source_filename = "match.arow"

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
  %a = alloca i32
  store i32 0, i32* %a
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp1, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  store i32 3, i32* %a
  %var_tmp2 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp2)
  br label %ifcont20

else:                                             ; preds = %entry
  %var_tmp3 = load i32, i32* %a
  %var_tmp4 = load i32, i32* %a
  %ifcond5 = icmp eq i32 %var_tmp4, 5
  br i1 %ifcond5, label %then6, label %else7

then6:                                            ; preds = %else
  store i32 2, i32* %a
  br label %ifcont19

else7:                                            ; preds = %else
  %var_tmp8 = load i32, i32* %a
  %var_tmp9 = load i32, i32* %a
  %ifcond10 = icmp eq i32 %var_tmp9, 10
  br i1 %ifcond10, label %then11, label %else12

then11:                                           ; preds = %else7
  br label %ifcont18

else12:                                           ; preds = %else7
  %var_tmp13 = load i32, i32* %a
  br i1 true, label %then14, label %else17

then14:                                           ; preds = %else12
  %var_tmp15 = load i32, i32* %a
  %call_tmp16 = call i1 @printnum(i32 %var_tmp15)
  br label %ifcont

else17:                                           ; preds = %else12
  br label %ifcont

ifcont:                                           ; preds = %else17, %then14
  br label %ifcont18

ifcont18:                                         ; preds = %ifcont, %then11
  br label %ifcont19

ifcont19:                                         ; preds = %ifcont18, %then6
  br label %ifcont20

ifcont20:                                         ; preds = %ifcont19, %then
  %var_tmp21 = load i32, i32* %a
  %var_tmp22 = load i32, i32* %a
  %ifcond23 = icmp ne i32 %var_tmp22, 0
  br i1 %ifcond23, label %then24, label %else25

then24:                                           ; preds = %ifcont20
  br label %ifcont26

else25:                                           ; preds = %ifcont20
  ret i32 0
  br label %ifcont26

ifcont26:                                         ; preds = %else25, %then24
  %var_tmp27 = load i32, i32* %a
  %call_tmp28 = call i1 @printnum(i32 %var_tmp27)
  ret i32 0
}
