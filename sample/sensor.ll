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

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false

entry1:                                           ; No predecessors!
  %array2 = alloca [2 x double]*, i32 2
  %i3 = alloca i32
  store i32 %i_arg, i32* %i3
  %mul_tmp5 = mul i32 %i_arg, 1000
  %call_tmp6 = call i1 @wait(i32 %mul_tmp5)
  ret i1 false
}

define i1 @SDA_DIROUT() {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 0)
  ret i1 false
}

define i1 @SCK_DIROUT() {
entry:
  %call_tmp = call i1 @gpioMode(i32 3, i32 0)
  ret i1 false
}

define i1 @SDA_DIRIN() {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 1)
  ret i1 false
}

define i1 @SCK_DIRIN() {
entry:
  %call_tmp = call i1 @gpioMode(i32 3, i32 1)
  ret i1 false
}

define i1 @SSS_DIRCL() {
entry:
  %call_tmp = call i1 @gpioMode(i32 2, i32 -1)
  %call_tmp1 = call i1 @gpioMode(i32 3, i32 -1)
  ret i1 false
}

define i1 @SDA_WRITE(i1 %i_arg) {
entry:
  %0 = zext i1 %i_arg to i32
  %call_tmp = call i1 @gpioWrite(i32 2, i32 %0)
  ret i1 false
}

define i1 @SCK_WRITE(i1 %i_arg) {
entry:
  %0 = zext i1 %i_arg to i32
  %call_tmp = call i1 @gpioWrite(i32 3, i32 %0)
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
  %ifcond = icmp sgt i32 %i_arg, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ %i_arg, %entry ], [ %sub_tmp, %loop ]
  %sub_tmp = sub i32 %i.0, 1
  %call_tmp = call i1 @wait(i32 500000)
  %ifcond5 = icmp sgt i32 %sub_tmp, 0
  br i1 %ifcond5, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i1 false
}

define i1 @send(i4 %size_arg, i9 %data_arg) {
entry:
  %call_tmp = call i1 @printnum(i32 1000)
  %0 = zext i9 %data_arg to i32
  %call_tmp1 = call i1 @printnum(i32 %0)
  %ifcond = icmp ugt i4 %size_arg, 0
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %size.0 = phi i4 [ %size_arg, %entry ], [ %sub_tmp21, %ifcont ]
  %call_tmp4 = call i1 @SCK_WRITE(i1 false)
  %sub_tmp = sub i4 %size.0, 1
  %1 = zext i4 %sub_tmp to i9
  %shl_tmp = shl i9 1, %1
  %and_tmp = and i9 %data_arg, %shl_tmp
  %sub_tmp9 = sub i4 %size.0, 1
  %2 = zext i4 %sub_tmp9 to i9
  %shl_tmp10 = shl i9 1, %2
  %and_tmp11 = and i9 %data_arg, %shl_tmp10
  %ifcond12 = icmp ne i9 %and_tmp11, 0
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
  %sub_tmp21 = sub i4 %size.0, 1
  %ifcond24 = icmp ugt i4 %sub_tmp21, 0
  br i1 %ifcond24, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  ret i1 false
}

define i9 @i2cStart(i8 %slave_arg, i9 %register_arg, i1 %RW_arg) {
entry:
  %call_tmp = call i1 @SDA_DIROUT()
  %call_tmp1 = call i1 @SDA_WRITE(i1 true)
  %call_tmp2 = call i1 @SCK_DIROUT()
  %call_tmp3 = call i1 @SCK_WRITE(i1 true)
  %call_tmp4 = call i1 @delay(i32 3)
  %call_tmp5 = call i1 @SDA_WRITE(i1 false)
  %call_tmp6 = call i1 @delay(i32 1)
  %call_tmp7 = call i1 @SCK_WRITE(i1 false)
  %call_tmp8 = call i1 @delay(i32 1)
  %0 = zext i8 %slave_arg to i9
  %call_tmp9 = call i1 @send(i4 7, i9 %0)
  %1 = zext i1 undef to i9
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
  %xor_tmp = xor i1 true, undef
  br i1 %xor_tmp, label %then22, label %else34

then22:                                           ; preds = %ifcont
  %call_tmp24 = call i1 @send(i4 -8, i9 %register_arg)
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
  %call_tmp = call i9 @i2cStart(i8 %slave_arg, i9 %register_arg, i1 false)
  %call_tmp3 = call i1 @send(i4 -8, i9 undef)
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
  %0 = trunc i9 %slave_arg to i8
  %call_tmp = call i9 @i2cStart(i8 %0, i9 %register_arg, i1 false)
  %1 = trunc i9 %slave_arg to i8
  %call_tmp4 = call i9 @i2cStart(i8 %1, i9 %register_arg, i1 true)
  %ifcond = icmp sgt i32 2, 0
  br i1 %ifcond, label %loop, label %afterloop39

loop:                                             ; preds = %ifcont, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %i.2, %ifcont ]
  %data.0 = phi i9 [ 0, %entry ], [ %data.2, %ifcont ]
  %transmission.0 = phi i32 [ 2, %entry ], [ %sub_tmp, %ifcont ]
  %call_tmp7 = call i1 @SDA_DIRIN()
  %ifcond11 = icmp slt i32 %i.0, 8
  br i1 %ifcond11, label %loop8, label %afterloop

loop8:                                            ; preds = %loop8, %loop
  %i.1 = phi i32 [ %i.0, %loop ], [ %add_tmp21, %loop8 ]
  %data.1 = phi i9 [ %data.0, %loop ], [ %add_tmp, %loop8 ]
  %call_tmp12 = call i1 @SCK_WRITE(i1 false)
  %call_tmp13 = call i1 @delay(i32 1)
  %call_tmp14 = call i1 @SDA_READ()
  %2 = zext i1 %call_tmp14 to i32
  %call_tmp15 = call i1 @printnum(i32 %2)
  %shl_tmp = shl i9 %data.1, 1
  %call_tmp17 = call i1 @SDA_READ()
  %3 = zext i1 %call_tmp17 to i9
  %add_tmp = add i9 %shl_tmp, %3
  %call_tmp18 = call i1 @SCK_WRITE(i1 true)
  %call_tmp19 = call i1 @delay(i32 1)
  %add_tmp21 = add i32 %i.1, 1
  %ifcond24 = icmp slt i32 %add_tmp21, 8
  br i1 %ifcond24, label %loop8, label %afterloop

afterloop:                                        ; preds = %loop8, %loop
  %i.2 = phi i32 [ %add_tmp21, %loop8 ], [ %i.0, %loop ]
  %data.2 = phi i9 [ %add_tmp, %loop8 ], [ %data.0, %loop ]
  %sub_tmp = sub i32 %transmission.0, 1
  %call_tmp26 = call i1 @SCK_WRITE(i1 false)
  %call_tmp27 = call i1 @SDA_DIROUT()
  %ifcond30 = icmp sgt i32 %sub_tmp, 0
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
  %ifcond38 = icmp sgt i32 %sub_tmp, 0
  br i1 %ifcond38, label %loop, label %afterloop39

afterloop39:                                      ; preds = %ifcont, %entry
  %data.3 = phi i9 [ %data.2, %ifcont ], [ 0, %entry ]
  %call_tmp40 = call i1 @SCK_WRITE(i1 false)
  %call_tmp41 = call i1 @SDA_WRITE(i1 false)
  %call_tmp42 = call i1 @delay(i32 1)
  %call_tmp43 = call i1 @SCK_WRITE(i1 true)
  %call_tmp44 = call i1 @delay(i32 1)
  %call_tmp45 = call i1 @SDA_WRITE(i1 true)
  %call_tmp46 = call i1 @delay(i32 1)
  %call_tmp47 = call i1 @SSS_DIRCL()
  ret i9 %data.3
}

define i32 @main() {
entry:
  %call_tmp = call i1 @printnum(i32 1024)
  %call_tmp1 = call i1 @confirm()
  %0 = zext i9 72 to i32
  %call_tmp2 = call i1 @printnum(i32 %0)
  %call_tmp5 = call i9 @i2cRead(i9 72, i9 3)
  %1 = zext i9 %call_tmp5 to i32
  %call_tmp6 = call i1 @printnum(i32 %1)
  %call_tmp7 = call i1 @Close()
  ret i32 0
}
