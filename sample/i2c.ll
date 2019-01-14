; ModuleID = 'i2c.arow'
source_filename = "i2c.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i8 @gpioRead(i32)

declare i1 @gpioClear(i32)

define i1 @send(i32 %gpio_num_arg, i8 %size_arg, i8 %data_arg) {
entry:
  %ifcond = icmp ugt i8 %size_arg, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %data.0 = phi i8 [ %data_arg, %entry ], [ %ashr_tmp, %ifcont ]
  %i.0 = phi i8 [ %size_arg, %entry ], [ %sub_tmp, %ifcont ]
  %sub_tmp = sub i8 %i.0, 1
  %shl_tmp = shl i8 1, %sub_tmp
  %and_tmp = and i8 %data.0, %shl_tmp
  %ifcond5 = icmp eq i8 %and_tmp, 0
  br i1 %ifcond5, label %then, label %else

then:                                             ; preds = %loop
  %call_tmp = call i1 @gpioWrite(i32 %gpio_num_arg, i32 1)
  br label %ifcont

else:                                             ; preds = %loop
  %call_tmp8 = call i1 @gpioWrite(i32 %gpio_num_arg, i32 0)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %ashr_tmp = ashr i8 %data.0, 1
  %call_tmp10 = call i1 @wait(i32 10)
  %ifcond12 = icmp ugt i8 %sub_tmp, 0
  br i1 %ifcond12, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  ret i1 false
}

define i8 @read() {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  %call_tmp1 = call i1 @send(i32 2, i8 2, i8 2)
  %call_tmp2 = call i1 @send(i32 2, i8 1, i8 0)
  ret i8 1
}
