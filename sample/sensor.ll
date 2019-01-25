; ModuleID = 'sensor.arow'
source_filename = "sensor.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

define i1 @sleep(i32 %i_arg) {
entry:
  %i = alloca i32
  store i32 %i_arg, i32* %i
  %var_tmp = load i32, i32* %i
  %mul_tmp = mul i32 %var_tmp, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false

entry1:                                           ; No predecessors!
  %i2 = alloca i32
  store i32 %i_arg, i32* %i2
  %var_tmp3 = load i32, i32* %i
  %mul_tmp4 = mul i32 %var_tmp3, 1000
  %call_tmp5 = call i1 @wait(i32 %mul_tmp4)
  ret i1 false
}

define i1 @SDA_DIROUT() {
entry:
  %call_tmp = call i1 @gpioMode(i32 22, i32 0)
  ret i1 false
}

define i1 @SCK_DIROUT() {
entry:
  %call_tmp = call i1 @gpioMode(i32 23, i32 0)
  ret i1 false
}

define i1 @SDA_DIRIN() {
entry:
  %call_tmp = call i1 @gpioMode(i32 22, i32 1)
  ret i1 false
}

define i1 @SCK_DIRIN() {
entry:
  %call_tmp = call i1 @gpioMode(i32 23, i32 1)
  ret i1 false
}

define i1 @SSS_DIRCL() {
entry:
  %call_tmp = call i1 @gpioMode(i32 22, i32 -1)
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 -1)
  ret i1 false
}

define i1 @SDA_WRITE(i1 %i_arg) {
entry:
  %i = alloca i1
  store i1 %i_arg, i1* %i
  %var_tmp = load i1, i1* %i
  %0 = zext i1 %var_tmp to i32
  %call_tmp = call i1 @gpioWrite(i32 22, i32 %0)
  ret i1 false
}

define i1 @SCK_WRITE(i1 %i_arg) {
entry:
  %i = alloca i1
  store i1 %i_arg, i1* %i
  %var_tmp = load i1, i1* %i
  %0 = zext i1 %var_tmp to i32
  %call_tmp = call i1 @gpioWrite(i32 23, i32 %0)
  ret i1 false
}

define i1 @SDA_READ() {
entry:
  %call_tmp = call i1 @gpioRead(i32 22)
  ret i1 false
}

define i1 @SCK_READ() {
entry:
  %call_tmp = call i1 @gpioRead(i32 23)
  ret i1 false
}

define i1 @delay(i32 %i_arg) {
entry:
  %i = alloca i32
  store i32 %i_arg, i32* %i
  %var_tmp = load i32, i32* %i
  %var_tmp1 = load i32, i32* %i
  %ifcond = icmp sgt i32 %var_tmp1, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp2 = load i32, i32* %i
  %sub_tmp = sub i32 %var_tmp2, 1
  store i32 %sub_tmp, i32* %i
  %call_tmp = call i1 @wait(i32 500000)
  %var_tmp3 = load i32, i32* %i
  %var_tmp4 = load i32, i32* %i
  %ifcond5 = icmp sgt i32 %var_tmp4, 0
  br i1 %ifcond5, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i1 false
}

define i1 @send(i4 %size_arg, i9 %data_arg) {
entry:
  %size = alloca i4
  store i4 %size_arg, i4* %size
  %data = alloca i9
  store i9 %data_arg, i9* %data
  %call_tmp = call i1 @printnum(i32 1000)
  %var_tmp = load i9, i9* %data
  %0 = zext i9 %var_tmp to i32
  %call_tmp1 = call i1 @printnum(i32 %0)
  %var_tmp2 = load i4, i4* %size
  %var_tmp3 = load i4, i4* %size
  %ifcond = icmp ugt i4 %var_tmp3, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %call_tmp4 = call i1 @SCK_WRITE(i1 false)
  %var_tmp5 = load i9, i9* %data
  %1 = zext i9 %var_tmp5 to i32
  %var_tmp6 = load i4, i4* %size
  %sub_tmp = sub i4 %var_tmp6, 1
  %2 = zext i4 %sub_tmp to i32
  %shl_tmp = shl i32 1, %2
  %and_tmp = and i32 %1, %shl_tmp
  %var_tmp7 = load i9, i9* %data
  %3 = zext i9 %var_tmp7 to i32
  %var_tmp8 = load i4, i4* %size
  %sub_tmp9 = sub i4 %var_tmp8, 1
  %4 = zext i4 %sub_tmp9 to i32
  %shl_tmp10 = shl i32 1, %4
  %and_tmp11 = and i32 %3, %shl_tmp10
  %ifcond12 = icmp ne i32 %and_tmp11, 0
  br i1 %ifcond12, label %then, label %else

then:                                             ; preds = %loop
  %call_tmp13 = call i1 @printnum(i32 1)
  %call_tmp14 = call i1 @SDA_WRITE(i1 true)
  br label %ifcont

else:                                             ; preds = %loop
  %call_tmp15 = call i1 @printnum(i32 0)
  %call_tmp16 = call i1 @SDA_WRITE(i1 false)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp17 = call i1 @delay(i32 1)
  %call_tmp18 = call i1 @SCK_WRITE(i1 true)
  %call_tmp19 = call i1 @delay(i32 1)
  %var_tmp20 = load i4, i4* %size
  %sub_tmp21 = sub i4 %var_tmp20, 1
  store i4 %sub_tmp21, i4* %size
  %var_tmp22 = load i4, i4* %size
  %var_tmp23 = load i4, i4* %size
  %ifcond24 = icmp ugt i4 %var_tmp23, 0
  br i1 %ifcond24, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  ret i1 false
}

define i9 @i2cStart(i8 %slave_arg, i9 %register_arg, i1 %RW_arg) {
entry:
  %slave = alloca i8
  store i8 %slave_arg, i8* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %RW = alloca i1
  %call_tmp = call i1 @SDA_DIROUT()
  %call_tmp1 = call i1 @SDA_WRITE(i1 true)
  %call_tmp2 = call i1 @SCK_DIROUT()
  %call_tmp3 = call i1 @SCK_WRITE(i1 true)
  %call_tmp4 = call i1 @delay(i32 3)
  %call_tmp5 = call i1 @SDA_WRITE(i1 false)
  %call_tmp6 = call i1 @delay(i32 1)
  %call_tmp7 = call i1 @SCK_WRITE(i1 false)
  %call_tmp8 = call i1 @delay(i32 1)
  %var_tmp = load i8, i8* %slave
  %0 = zext i8 %var_tmp to i9
  %call_tmp9 = call i1 @send(i4 7, i9 %0)
  %var_tmp10 = load i1, i1* %RW
  %1 = zext i1 %var_tmp10 to i9
  %call_tmp11 = call i1 @send(i4 1, i9 %1)
  %call_tmp12 = call i1 @SCK_WRITE(i1 false)
  %call_tmp13 = call i1 @SDA_WRITE(i1 false)
  %call_tmp14 = call i1 @SDA_DIRIN()
  %call_tmp15 = call i1 @delay(i32 1)
  %call_tmp16 = call i1 @SCK_WRITE(i1 true)
  %call_tmp17 = call i1 @SDA_READ()
  %call_tmp18 = call i1 @SDA_READ()
  %ifcond = icmp eq i1 %call_tmp18, true
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp19 = call i1 @printnum(i32 123351)
  ret i9 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp20 = call i1 @delay(i32 1)
  %var_tmp21 = load i1, i1* %RW
  %xor_tmp = xor i1 true, %var_tmp21
  br i1 %xor_tmp, label %then22, label %else34

then22:                                           ; preds = %ifcont
  %var_tmp23 = load i9, i9* %register
  %call_tmp24 = call i1 @send(i4 -8, i9 %var_tmp23)
  %call_tmp25 = call i1 @delay(i32 1)
  %call_tmp26 = call i1 @SDA_READ()
  %call_tmp27 = call i1 @SDA_READ()
  %ifcond28 = icmp eq i1 %call_tmp27, true
  br i1 %ifcond28, label %then29, label %else31

then29:                                           ; preds = %then22
  %call_tmp30 = call i1 @printnum(i32 123352)
  ret i9 0
  br label %ifcont32

else31:                                           ; preds = %then22
  br label %ifcont32

ifcont32:                                         ; preds = %else31, %then29
  %call_tmp33 = call i1 @delay(i32 1)
  br label %ifcont35

else34:                                           ; preds = %ifcont
  br label %ifcont35

ifcont35:                                         ; preds = %else34, %ifcont32
  ret i9 0
}

define i9 @i2cWrite(i8 %slave_arg, i9 %register_arg, i9 %data_arg) {
entry:
  %slave = alloca i8
  store i8 %slave_arg, i8* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %data = alloca i9
  %var_tmp = load i8, i8* %slave
  %var_tmp1 = load i9, i9* %register
  %call_tmp = call i9 @i2cStart(i8 %var_tmp, i9 %var_tmp1, i1 false)
  %var_tmp2 = load i9, i9* %data
  %call_tmp3 = call i1 @send(i4 -8, i9 %var_tmp2)
  %call_tmp4 = call i1 @delay(i32 1)
  %call_tmp5 = call i1 @SDA_DIRIN()
  %call_tmp6 = call i1 @SDA_READ()
  %call_tmp7 = call i1 @SDA_READ()
  %ifcond = icmp eq i1 %call_tmp7, true
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp8 = call i1 @printnum(i32 99999999)
  ret i9 0
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp9 = call i1 @SDA_DIROUT()
  %call_tmp10 = call i1 @delay(i32 1)
  %call_tmp11 = call i1 @SCK_WRITE(i1 false)
  %call_tmp12 = call i1 @SDA_WRITE(i1 false)
  %call_tmp13 = call i1 @delay(i32 1)
  %call_tmp14 = call i1 @SCK_WRITE(i1 true)
  %call_tmp15 = call i1 @delay(i32 1)
  %call_tmp16 = call i1 @SDA_WRITE(i1 true)
  %call_tmp17 = call i1 @delay(i32 1)
  %call_tmp18 = call i1 @SSS_DIRCL()
  ret i9 0
}

define i9 @i2cRead(i9 %slave_arg, i9 %register_arg) {
entry:
  %slave = alloca i9
  store i9 %slave_arg, i9* %slave
  %register = alloca i9
  store i9 %register_arg, i9* %register
  %var_tmp = load i9, i9* %slave
  %0 = trunc i9 %var_tmp to i8
  %var_tmp1 = load i9, i9* %register
  %call_tmp = call i9 @i2cStart(i8 %0, i9 %var_tmp1, i1 false)
  %var_tmp2 = load i9, i9* %slave
  %1 = trunc i9 %var_tmp2 to i8
  %var_tmp3 = load i9, i9* %register
  %call_tmp4 = call i9 @i2cStart(i8 %1, i9 %var_tmp3, i1 true)
  %i = alloca i32
  store i32 0, i32* %i
  %data = alloca i9
  store i9 0, i9* %data
  %transmission = alloca i32
  store i32 2, i32* %transmission
  %var_tmp5 = load i32, i32* %transmission
  %var_tmp6 = load i32, i32* %transmission
  %ifcond = icmp sgt i32 %var_tmp6, 0
  br i1 %ifcond, label %loop, label %afterloop39

loop:                                             ; preds = %ifcont, %entry
  %call_tmp7 = call i1 @SDA_DIRIN()
  %var_tmp9 = load i32, i32* %i
  %var_tmp10 = load i32, i32* %i
  %ifcond11 = icmp slt i32 %var_tmp10, 8
  br i1 %ifcond11, label %loop8, label %afterloop

loop8:                                            ; preds = %loop8, %loop
  %call_tmp12 = call i1 @SCK_WRITE(i1 false)
  %call_tmp13 = call i1 @delay(i32 1)
  %call_tmp14 = call i1 @SDA_READ()
  %2 = zext i1 %call_tmp14 to i32
  %call_tmp15 = call i1 @printnum(i32 %2)
  %var_tmp16 = load i9, i9* %data
  %3 = zext i9 %var_tmp16 to i32
  %shl_tmp = shl i32 %3, 1
  %call_tmp17 = call i1 @SDA_READ()
  %4 = zext i1 %call_tmp17 to i32
  %add_tmp = add i32 %shl_tmp, %4
  %5 = trunc i32 %add_tmp to i9
  store i9 %5, i9* %data
  %call_tmp18 = call i1 @SCK_WRITE(i1 true)
  %call_tmp19 = call i1 @delay(i32 1)
  %var_tmp20 = load i32, i32* %i
  %add_tmp21 = add i32 %var_tmp20, 1
  store i32 %add_tmp21, i32* %i
  %var_tmp22 = load i32, i32* %i
  %var_tmp23 = load i32, i32* %i
  %ifcond24 = icmp slt i32 %var_tmp23, 8
  br i1 %ifcond24, label %loop8, label %afterloop

afterloop:                                        ; preds = %loop8, %loop
  %var_tmp25 = load i32, i32* %transmission
  %sub_tmp = sub i32 %var_tmp25, 1
  store i32 %sub_tmp, i32* %transmission
  %call_tmp26 = call i1 @SCK_WRITE(i1 false)
  %call_tmp27 = call i1 @SDA_DIROUT()
  %var_tmp28 = load i32, i32* %transmission
  %var_tmp29 = load i32, i32* %transmission
  %ifcond30 = icmp sgt i32 %var_tmp29, 0
  br i1 %ifcond30, label %then, label %else

then:                                             ; preds = %afterloop
  %call_tmp31 = call i1 @SDA_WRITE(i1 false)
  br label %ifcont

else:                                             ; preds = %afterloop
  %call_tmp32 = call i1 @SDA_WRITE(i1 true)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp33 = call i1 @delay(i32 1)
  %call_tmp34 = call i1 @SCK_WRITE(i1 true)
  %call_tmp35 = call i1 @delay(i32 1)
  %var_tmp36 = load i32, i32* %transmission
  %var_tmp37 = load i32, i32* %transmission
  %ifcond38 = icmp sgt i32 %var_tmp37, 0
  br i1 %ifcond38, label %loop, label %afterloop39

afterloop39:                                      ; preds = %ifcont, %entry
  %call_tmp40 = call i1 @SCK_WRITE(i1 false)
  %call_tmp41 = call i1 @SDA_WRITE(i1 false)
  %call_tmp42 = call i1 @delay(i32 1)
  %call_tmp43 = call i1 @SCK_WRITE(i1 true)
  %call_tmp44 = call i1 @delay(i32 1)
  %call_tmp45 = call i1 @SDA_WRITE(i1 true)
  %call_tmp46 = call i1 @delay(i32 1)
  %call_tmp47 = call i1 @SSS_DIRCL()
  %var_tmp48 = load i9, i9* %data
  ret i9 %var_tmp48
}

define i32 @main() {
entry:
  %slave = alloca i9
  store i9 72, i9* %slave
  %register = alloca i9
  store i9 3, i9* %register
  %call_tmp = call i1 @printnum(i32 1024)
  %call_tmp1 = call i1 @confirm()
  %var_tmp = load i9, i9* %slave
  %0 = zext i9 %var_tmp to i32
  %call_tmp2 = call i1 @printnum(i32 %0)
  %var_tmp3 = load i9, i9* %slave
  %var_tmp4 = load i9, i9* %register
  %call_tmp5 = call i9 @i2cRead(i9 %var_tmp3, i9 %var_tmp4)
  %1 = zext i9 %call_tmp5 to i32
  %call_tmp6 = call i1 @printnum(i32 %1)
  %call_tmp7 = call i1 @Close()
  ret i32 0
}
