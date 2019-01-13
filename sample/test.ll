; ModuleID = 'test.arow'
source_filename = "test.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @test(i32 %j_arg) {
entry:
  %array = alloca [2 x double]*, i32 2
  %j = alloca i32
  store i32 %j_arg, i32* %j
  %i = alloca i32
  %var_tmp = load i32, i32* %j
  %mul_tmp = mul i32 %var_tmp, 10
  store i32 %mul_tmp, i32* %i
  %var_tmp1 = load i32, i32* %i
  ret i32 %var_tmp1
}

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %i = alloca i32
  store i32 10, i32* %i
  %var_tmp = load i32, i32* %i
  %call_tmp = call i32 @test(i32 %var_tmp)
  %call_tmp1 = call i1 @printnum(i32 %call_tmp)
  ret i32 0
}
