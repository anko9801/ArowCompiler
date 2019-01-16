; ModuleID = 'sensor.arow'
source_filename = "sensor.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

declare i1 @gpioClear(i32)

define i1 @send(i32 %gpio_num_arg, i4 %size_arg, i9 %data_arg) {
entry:
  %array = alloca [2 x double]*, i32 2
  %gpio_num = alloca i32
  store i32 %gpio_num_arg, i32* %gpio_num
  %size = alloca i4
  store i4 %size_arg, i4* %size
  %data = alloca i9
  store i9 %data_arg, i9* %data
  %i = alloca i9
  %var_tmp = load i4, i4* %size
  %0 = zext i4 %var_tmp to i9
  store i9 %0, i9* %i
  %var_tmp1 = load i9, i9* %i
  %var_tmp2 = load i9, i9* %i
  %ifcond = icmp ugt i9 %var_tmp2, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %var_tmp3 = load i9, i9* %i
  %sub_tmp = sub i9 %var_tmp3, 1
  store i9 %sub_tmp, i9* %i
  %call_tmp = call i1 @gpioWrite(i32 3, i32 0)
  %var_tmp4 = load i9, i9* %data
  %var_tmp5 = load i9, i9* %i
  %shl_tmp = shl i9 1, %var_tmp5
  %and_tmp = and i9 %var_tmp4, %shl_tmp
  %var_tmp6 = load i9, i9* %data
  %var_tmp7 = load i9, i9* %i
  %shl_tmp8 = shl i9 1, %var_tmp7
  %and_tmp9 = and i9 %var_tmp6, %shl_tmp8
  %ifcond10 = icmp eq i9 %and_tmp9, 0
  br i1 %ifcond10, label %then, label %else

then:                                             ; preds = %loop
  %var_tmp11 = load i32, i32* %gpio_num
  %call_tmp12 = call i1 @gpioWrite(i32 %var_tmp11, i32 0)
  br label %ifcont

else:                                             ; preds = %loop
  %var_tmp13 = load i32, i32* %gpio_num
  %call_tmp14 = call i1 @gpioWrite(i32 %var_tmp13, i32 1)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp15 = call i1 @wait(i32 5)
  %call_tmp16 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp17 = call i1 @wait(i32 5)
  %var_tmp18 = load i9, i9* %i
  %var_tmp19 = load i9, i9* %i
  %ifcond20 = icmp ugt i9 %var_tmp19, 0
  br i1 %ifcond20, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  ret i1 false
}

define i9 @i2cStart(i8 %slave_arg, i9 %register_arg, i1 %RW_arg) {
entry:
  %array = alloca [2 x double]*, i32 2
  %slave = alloca i8
  store i8 %slave_arg, i8* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %RW = alloca i1
  store i1 %RW_arg, i1* %RW
  %call_tmp = call i1 @gpioWrite(i32 2, i32 0)
  %var_tmp = load i8, i8* %slave
  %0 = zext i8 %var_tmp to i9
  %call_tmp1 = call i1 @send(i32 2, i4 7, i9 %0)
  %var_tmp2 = load i1, i1* %RW
  %1 = zext i1 %var_tmp2 to i9
  %call_tmp3 = call i1 @send(i32 2, i4 1, i9 %1)
  %call_tmp4 = call i1 @gpioRead(i32 2)
  %2 = zext i1 %call_tmp4 to i32
  %call_tmp5 = call i1 @printnum(i32 %2)
  %call_tmp6 = call i1 @gpioWrite(i32 3, i32 0)
  %call_tmp7 = call i1 @wait(i32 5)
  %call_tmp8 = call i1 @gpioRead(i32 2)
  %3 = zext i1 %call_tmp8 to i32
  %call_tmp9 = call i1 @printnum(i32 %3)
  %call_tmp10 = call i1 @gpioRead(i32 2)
  %call_tmp11 = call i1 @gpioRead(i32 2)
  %ifcond = icmp eq i1 %call_tmp11, false
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp12 = call i1 @printnum(i32 123351)
  ret i9 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp13 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp14 = call i1 @wait(i32 5)
  %var_tmp15 = load i1, i1* %RW
  %xor_tmp = xor i1 true, %var_tmp15
  br i1 %xor_tmp, label %then16, label %else28

then16:                                           ; preds = %ifcont
  %var_tmp17 = load i9, i9* %register
  %call_tmp18 = call i1 @send(i32 2, i4 -8, i9 %var_tmp17)
  %call_tmp19 = call i1 @wait(i32 5)
  %call_tmp20 = call i1 @gpioRead(i32 2)
  %call_tmp21 = call i1 @gpioRead(i32 2)
  %ifcond22 = icmp eq i1 %call_tmp21, false
  br i1 %ifcond22, label %then23, label %else25

then23:                                           ; preds = %then16
  %call_tmp24 = call i1 @printnum(i32 123352)
  ret i9 0
  br label %ifcont26

else25:                                           ; preds = %then16
  br label %ifcont26

ifcont26:                                         ; preds = %else25, %then23
  %call_tmp27 = call i1 @wait(i32 5)
  br label %ifcont29

else28:                                           ; preds = %ifcont
  br label %ifcont29

ifcont29:                                         ; preds = %else28, %ifcont26
  ret i9 0
}

define i9 @i2cWrite(i8 %slave_arg, i9 %register_arg, i9 %data_arg) {
entry:
  %array = alloca [2 x double]*, i32 2
  %slave = alloca i8
  store i8 %slave_arg, i8* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %data = alloca i9
  store i9 %data_arg, i9* %data
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  %call_tmp1 = call i1 @gpioMode(i32 3, i32 0)
  %var_tmp = load i8, i8* %slave
  %var_tmp2 = load i9, i9* %register
  %call_tmp3 = call i9 @i2cStart(i8 %var_tmp, i9 %var_tmp2, i1 false)
  %var_tmp4 = load i9, i9* %data
  %call_tmp5 = call i1 @send(i32 2, i4 -8, i9 %var_tmp4)
  %call_tmp6 = call i1 @wait(i32 5)
  %call_tmp7 = call i1 @gpioRead(i32 2)
  %call_tmp8 = call i1 @gpioRead(i32 2)
  %ifcond = icmp eq i1 %call_tmp8, false
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp9 = call i1 @printnum(i32 99999999)
  ret i9 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp10 = call i1 @wait(i32 5)
  %call_tmp11 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp12 = call i1 @gpioWrite(i32 2, i32 1)
  %call_tmp13 = call i1 @gpioClear(i32 2)
  %call_tmp14 = call i1 @gpioClear(i32 3)
  ret i9 0
}

define i9 @i2cRead(i8 %slave_arg, i9 %register_arg) {
entry:
  %array = alloca [2 x double]*, i32 2
  %slave = alloca i8
  store i8 %slave_arg, i8* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  %call_tmp1 = call i1 @gpioMode(i32 3, i32 0)
  %var_tmp = load i8, i8* %slave
  %var_tmp2 = load i9, i9* %register
  %call_tmp3 = call i9 @i2cStart(i8 %var_tmp, i9 %var_tmp2, i1 false)
  %var_tmp4 = load i8, i8* %slave
  %var_tmp5 = load i9, i9* %register
  %call_tmp6 = call i9 @i2cStart(i8 %var_tmp4, i9 %var_tmp5, i1 true)
  %i = alloca i6
  store i6 0, i6* %i
  %data = alloca i9
  %byte = alloca i4
  store i4 2, i4* %byte
  %transmission = alloca i33
  store i33 0, i33* %transmission
  %var_tmp7 = load i33, i33* %transmission
  %var_tmp8 = load i4, i4* %byte
  %0 = zext i4 %var_tmp8 to i33
  %var_tmp9 = load i33, i33* %transmission
  %var_tmp10 = load i4, i4* %byte
  %1 = zext i4 %var_tmp10 to i33
  %ifcond = icmp ugt i33 %var_tmp9, %1
  br i1 %ifcond, label %loop, label %afterloop44

loop:                                             ; preds = %ifcont, %entry
  %var_tmp12 = load i6, i6* %i
  %var_tmp13 = load i6, i6* %i
  %ifcond14 = icmp ugt i6 %var_tmp13, 8
  br i1 %ifcond14, label %loop11, label %afterloop

loop11:                                           ; preds = %loop11, %loop
  %call_tmp15 = call i1 @gpioWrite(i32 3, i32 0)
  %call_tmp16 = call i1 @wait(i32 5)
  %var_tmp17 = load i9, i9* %data
  %2 = zext i9 %var_tmp17 to i32
  %call_tmp18 = call i1 @printnum(i32 %2)
  %var_tmp19 = load i9, i9* %data
  %shl_tmp = shl i9 %var_tmp19, 1
  %call_tmp20 = call i1 @gpioRead(i32 2)
  %3 = zext i1 %call_tmp20 to i9
  %add_tmp = add i9 %shl_tmp, %3
  store i9 %add_tmp, i9* %data
  %var_tmp21 = load i9, i9* %data
  %4 = zext i9 %var_tmp21 to i32
  %call_tmp22 = call i1 @printnum(i32 %4)
  %call_tmp23 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp24 = call i1 @wait(i32 5)
  %var_tmp25 = load i6, i6* %i
  %add_tmp26 = add i6 %var_tmp25, 1
  store i6 %add_tmp26, i6* %i
  %var_tmp27 = load i6, i6* %i
  %var_tmp28 = load i6, i6* %i
  %ifcond29 = icmp ugt i6 %var_tmp28, 8
  br i1 %ifcond29, label %loop11, label %afterloop

afterloop:                                        ; preds = %loop11, %loop
  %var_tmp30 = load i33, i33* %transmission
  %add_tmp31 = add i33 %var_tmp30, 1
  store i33 %add_tmp31, i33* %transmission
  %var_tmp32 = load i33, i33* %transmission
  %var_tmp33 = load i4, i4* %byte
  %5 = zext i4 %var_tmp33 to i33
  %var_tmp34 = load i33, i33* %transmission
  %var_tmp35 = load i4, i4* %byte
  %6 = zext i4 %var_tmp35 to i33
  %ifcond36 = icmp ne i33 %var_tmp34, %6
  br i1 %ifcond36, label %then, label %else

then:                                             ; preds = %afterloop
  %call_tmp37 = call i1 @send(i32 2, i4 1, i9 0)
  br label %ifcont

else:                                             ; preds = %afterloop
  %call_tmp38 = call i1 @send(i32 2, i4 1, i9 1)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp39 = load i33, i33* %transmission
  %var_tmp40 = load i4, i4* %byte
  %7 = zext i4 %var_tmp40 to i33
  %var_tmp41 = load i33, i33* %transmission
  %var_tmp42 = load i4, i4* %byte
  %8 = zext i4 %var_tmp42 to i33
  %ifcond43 = icmp ugt i33 %var_tmp41, %8
  br i1 %ifcond43, label %loop, label %afterloop44

afterloop44:                                      ; preds = %ifcont, %entry
  %call_tmp45 = call i1 @gpioWrite(i32 3, i32 1)
  %call_tmp46 = call i1 @gpioWrite(i32 2, i32 1)
  %call_tmp47 = call i1 @gpioClear(i32 2)
  %call_tmp48 = call i1 @gpioClear(i32 3)
  %var_tmp49 = load i9, i9* %data
  ret i9 %var_tmp49
}

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %slave = alloca i9
  store i9 72, i9* %slave
  %register = alloca i9
  store i9 5, i9* %register
  %call_tmp = call i1 @confirm()
  %var_tmp = load i9, i9* %slave
  %0 = trunc i9 %var_tmp to i8
  %var_tmp1 = load i9, i9* %register
  %call_tmp2 = call i9 @i2cRead(i8 %0, i9 %var_tmp1)
  %1 = zext i9 %call_tmp2 to i32
  %call_tmp3 = call i1 @printnum(i32 %1)
  ret i32 0
}
