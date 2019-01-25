; ModuleID = 'test6.arow'
source_filename = "test6.arow"

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
  store i32 4, i32* %a
  %b = alloca i32
  store i32 5, i32* %b
  br i1 false, label %then, label %else26

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
  %var_tmp3 = load i32, i32* %b
  %var_tmp4 = load i32, i32* %b
  %ifcond5 = icmp ne i32 %var_tmp4, 0
  br i1 %ifcond5, label %then6, label %else7

then6:                                            ; preds = %ifcont
  br label %ifcont8

else7:                                            ; preds = %ifcont
  ret i32 0
  br label %ifcont8

ifcont8:                                          ; preds = %else7, %then6
  %var_tmp9 = load i32, i32* %a
  %var_tmp10 = load i32, i32* %b
  %add_tmp = add i32 %var_tmp9, %var_tmp10
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %var_tmp11 = load i32, i32* %a
  %var_tmp12 = load i32, i32* %a
  %ifcond13 = icmp ne i32 %var_tmp12, 0
  br i1 %ifcond13, label %then14, label %else15

then14:                                           ; preds = %ifcont8
  br label %ifcont16

else15:                                           ; preds = %ifcont8
  ret i32 0
  br label %ifcont16

ifcont16:                                         ; preds = %else15, %then14
  %var_tmp17 = load i32, i32* %b
  %var_tmp18 = load i32, i32* %b
  %ifcond19 = icmp ne i32 %var_tmp18, 0
  br i1 %ifcond19, label %then20, label %else21

then20:                                           ; preds = %ifcont16
  br label %ifcont22

else21:                                           ; preds = %ifcont16
  ret i32 0
  br label %ifcont22

ifcont22:                                         ; preds = %else21, %then20
  %var_tmp23 = load i32, i32* %a
  %var_tmp24 = load i32, i32* %b
  %sub_tmp = sub i32 %var_tmp23, %var_tmp24
  %call_tmp25 = call i1 @printnum(i32 %sub_tmp)
  br label %ifcont47

else26:                                           ; preds = %entry
  %ak = alloca i32
  %var_tmp27 = load i32, i32* %a
  %var_tmp28 = load i32, i32* %a
  %ifcond29 = icmp ne i32 %var_tmp28, 0
  br i1 %ifcond29, label %then30, label %else31

then30:                                           ; preds = %else26
  br label %ifcont32

else31:                                           ; preds = %else26
  ret i32 0
  br label %ifcont32

ifcont32:                                         ; preds = %else31, %then30
  %var_tmp33 = load i32, i32* %a
  store i32 %var_tmp33, i32* %ak
  %bk = alloca i32
  %var_tmp34 = load i32, i32* %b
  %var_tmp35 = load i32, i32* %b
  %ifcond36 = icmp ne i32 %var_tmp35, 0
  br i1 %ifcond36, label %then37, label %else38

then37:                                           ; preds = %ifcont32
  br label %ifcont39

else38:                                           ; preds = %ifcont32
  ret i32 0
  br label %ifcont39

ifcont39:                                         ; preds = %else38, %then37
  %var_tmp40 = load i32, i32* %b
  store i32 %var_tmp40, i32* %bk
  %var_tmp41 = load i32, i32* %ak
  %var_tmp42 = load i32, i32* %bk
  %mul_tmp = mul i32 %var_tmp41, %var_tmp42
  %call_tmp43 = call i1 @printnum(i32 %mul_tmp)
  %var_tmp44 = load i32, i32* %ak
  %var_tmp45 = load i32, i32* %bk
  %div_tmp = sdiv i32 %var_tmp44, %var_tmp45
  %call_tmp46 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont47

ifcont47:                                         ; preds = %ifcont39, %ifcont22
  ret i32 0
}
