; ModuleID = 'test.arow'
source_filename = "test.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @test(i32 %j_arg) {
entry:
  %mul_tmp = mul i32 %j_arg, 10
  ret i32 %mul_tmp
}

define i32 @main() {
entry:
  %call_tmp = call i32 @test(i32 10)
  %call_tmp1 = call i1 @printnum(i32 %call_tmp)
  ret i32 0
}
