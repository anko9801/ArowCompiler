; ModuleID = 'i2c.arow'
source_filename = "i2c.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

declare i1 @gpioClear(i32)

define i1 @send(i32 %gpio_num_arg, i3 %size_arg, i8 %data_arg) {
entry:
  %0 = zext i3 %size_arg to i8
  %ifcond = icmp ugt i8 %0, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %i.0 = phi i8 [ %0, %entry ], [ %sub_tmp, %ifcont ]
  %sub_tmp = sub i8 %i.0, 1
  %call_tmp = call i1 @gpioWrite(i32 3, i32 0)
  %shl_tmp = shl i8 1, %sub_tmp
  %and_tmp = and i8 %data_arg, %shl_tmp
  %shl_tmp8 = shl i8 1, %sub_tmp
  %and_tmp9 = and i8 %data_arg, %shl_tmp8
  %ifcond10 = icmp eq i8 %and_tmp9, 0
  br i1 %ifcond10, label %then, label %else

then:                                             ; preds = %loop
  %call_tmp12 = call i1 @gpioWrite(i32 %gpio_num_arg, i32 0)
  br label %ifcont

else:                                             ; preds = %loop
  %call_tmp14 = call i1 @gpioWrite(i32 %gpio_num_arg, i32 1)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp15 = call i1 @wait(i32 5)
  %call_tmp16 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp17 = call i1 @wait(i32 5)
  %ifcond20 = icmp ugt i8 %sub_tmp, 0
  br i1 %ifcond20, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  ret i1 false
}

define i8 @i2cStart(i7 %slave_arg, i8 %register_arg, i1 %RW_arg) {
entry:
  %call_tmp = call i1 @send(i32 2, i3 3, i8 4)
  %0 = zext i7 %slave_arg to i8
  %call_tmp1 = call i1 @send(i32 2, i3 -1, i8 %0)
  %1 = zext i1 %RW_arg to i8
  %call_tmp3 = call i1 @send(i32 2, i3 1, i8 %1)
  %call_tmp4 = call i1 @gpioWrite(i32 3, i32 0)
  %call_tmp5 = call i1 @wait(i32 5)
  %call_tmp6 = call i1 @gpioRead(i32 2)
  br i1 %call_tmp6, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp7 = call i1 @printnum(i32 99999999)
  ret i8 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp8 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp9 = call i1 @wait(i32 5)
  %xor_tmp = xor i1 true, %RW_arg
  br i1 %xor_tmp, label %then11, label %else22

then11:                                           ; preds = %ifcont
  %call_tmp13 = call i1 @send(i32 2, i3 0, i8 %register_arg)
  %call_tmp14 = call i1 @wait(i32 5)
  %call_tmp15 = call i1 @gpioRead(i32 2)
  %call_tmp16 = call i1 @gpioRead(i32 2)
  %ifcond = icmp eq i1 %call_tmp16, true
  br i1 %ifcond, label %then17, label %else19

then17:                                           ; preds = %then11
  %call_tmp18 = call i1 @printnum(i32 99999999)
  ret i8 0
  br label %ifcont20

else19:                                           ; preds = %then11
  br label %ifcont20

ifcont20:                                         ; preds = %else19, %then17
  %call_tmp21 = call i1 @wait(i32 5)
  br label %ifcont23

else22:                                           ; preds = %ifcont
  br label %ifcont23

ifcont23:                                         ; preds = %else22, %ifcont20
  ret i8 0
}

define i8 @i2cWrite(i7 %slave_arg, i8 %register_arg, i8 %data_arg) {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  %call_tmp1 = call i1 @gpioMode(i32 3, i32 0)
  %call_tmp3 = call i8 @i2cStart(i7 %slave_arg, i8 %register_arg, i1 false)
  %call_tmp5 = call i1 @send(i32 2, i3 0, i8 %data_arg)
  %call_tmp6 = call i1 @wait(i32 5)
  %call_tmp7 = call i1 @gpioRead(i32 2)
  %call_tmp8 = call i1 @gpioRead(i32 2)
  %ifcond = icmp eq i1 %call_tmp8, false
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp9 = call i1 @printnum(i32 99999999)
  ret i8 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp10 = call i1 @wait(i32 5)
  %call_tmp11 = call i1 @send(i32 2, i3 3, i8 3)
  ret i8 0
}

define i8 @i2cRead(i7 %slave_arg, i8 %register_arg) {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  %call_tmp1 = call i1 @gpioMode(i32 3, i32 0)
  %call_tmp3 = call i8 @i2cStart(i7 %slave_arg, i8 %register_arg, i1 false)
  %call_tmp6 = call i8 @i2cStart(i7 %slave_arg, i8 %register_arg, i1 true)
  %ifcond = icmp ugt i4 0, -8
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i4 [ 0, %entry ], [ %add_tmp16, %loop ]
  %data.0 = phi i8 [ undef, %entry ], [ %add_tmp, %loop ]
  %call_tmp9 = call i1 @gpioWrite(i32 3, i32 0)
  %call_tmp10 = call i1 @wait(i32 5)
  %shl_tmp = shl i8 %data.0, 1
  %call_tmp12 = call i1 @gpioRead(i32 2)
  %0 = zext i1 %call_tmp12 to i8
  %add_tmp = add i8 %shl_tmp, %0
  %call_tmp13 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp14 = call i1 @wait(i32 5)
  %add_tmp16 = add i4 %i.0, 1
  %ifcond19 = icmp ugt i4 %add_tmp16, -8
  br i1 %ifcond19, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %data.1 = phi i8 [ %add_tmp, %loop ], [ undef, %entry ]
  %call_tmp20 = call i1 @send(i32 2, i3 1, i8 1)
  %call_tmp21 = call i1 @send(i32 2, i3 3, i8 3)
  ret i8 %data.1
}
