# Arow

## Sample

i32 test(int j) {
	i32 i = j * 10;
	return i;
}

i32 main() {
	int i = 10;
    printnum(test(i));
	return 0;
}


i32 main() {
	i32 k = 23;
	printnum(k);
	return 0;
}

i32 main() {
	bool k = true;
	i32 a = 32;
	if k {
		printnum(a);
	}
	return 0;
}

i32 main() {
	i32 a = 4;
	i32 b = 5;
	if true {
		printnum(a + b);
		printnum(a - b);
	}else{
		printnum(a * b);
		printnum(a / b);
	}
	return 0;
}


## コンパイル
/bin/dcc [file] [option], ...

option
-o : filename 
