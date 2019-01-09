; ModuleID = 'test2.arow'
source_filename = "test2.arow"

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i32
  %b = alloca i1
  store i1 false, i1* %b
  ret i32 0
}
