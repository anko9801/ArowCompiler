; ModuleID = 'diffint.arow'
source_filename = "diffint.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define float @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca half
  store half 0xH4600, half* %a
  %var_tmp = load half, half* %a
  %0 = fpext half %var_tmp to float
  ret float %0
}
