Test case 1:  
	Order Of Instructions Removed:
	  %useless3 = add nsw i32 %useless1, %useless2
	  %useless1 = load i32* %a, align 4
	  %useless2 = load i32* %b, align 4
	  br label %unreachable
	  %unreachable1 = load i32* %a, align 4
	  %unreachable2 = load i32* %b, align 4
	  %unreachable3 = add nsw i32 %unreachable1, %unreachable2
	  br label %if.end

	Order Of BasicBlocks Removed:
		unreachable

Test case 2:
	Order Of Instructions Removed:
	  %useless13 = add nsw i32 %useless11, %useless12
	  %useless33 = add nsw i32 %useless31, %useless32
	  %useless23 = add nsw i32 %useless21, %useless22
	  %useless11 = load i32* %a, align 4
	  %useless12 = load i32* %b, align 4
	  %useless21 = load i32* %a, align 4
	  %useless22 = load i32* %b, align 4
	  %useless31 = load i32* %a, align 4
	  %useless32 = load i32* %b, align 4

Test case 3:
	Order of Instructions Removed:
		%uselessA = alloca i32, align 4
  		%useless3 = mul nsw i32 %useless1, %useless2
  		%useless1 = add nsw i32 %useful1, 2
  		%useless2 = mul nsw i32 %useful1, %useful2


To run the test cases:
	cd Release+Asserts/bin
	./llvm-as testi.ll -o testi.bc
	./opt -load ../lib/ADCE.so -ADCE testi.bc -o outputi.bc






