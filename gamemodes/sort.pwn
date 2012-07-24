#include <a_samp>

stock insertion_sort(array[], left, right) {
	for (new j = left + 1; j <= right; j++) {
		new key = array[j], i = j - 1;
		
		while (i >= left && array[i] > key) {
			array[i + 1] = array[i];
			
			i--;
		}
		
		array[i + 1] = key;
	}
}

stock QSort(numbers[], left, right)
{
	new
		pivot = numbers[left],
		l_hold = left,
		r_hold = right
	;
	
	if (right - left <= 6) {
		insertion_sort(numbers, left, right);
		
		return;
	}
	
	while (left < right) {
		while (numbers[right] >= pivot && left < right)
			right--;
		
		if (left != right) {
			numbers[left] = numbers[right];
			left++;
		}
		
		while (numbers[left] <= pivot && left < right)
			left++;
		
		if (left != right) {
			numbers[right] = numbers[left];
			right--;
		}
	}
	
	numbers[left] = pivot;
	pivot = left;
	left = l_hold;
	right = r_hold;
	
	if (left < pivot)  QSort(numbers,  left, pivot - 1);
	if (right > pivot) QSort(numbers, pivot + 1, right);
}

stock QuickSort_old(array[], size = sizeof(array)) {
	QSort(array, 0, size - 1);
}

stock __tmp;

stock QuickSort(numbers[], size = sizeof(numbers)) {
	#define PUSH(%1,%2)	     (stack[top][0] = (%1), stack[top][1] = (%2), top++)
	#define	POP(%1,%2)	     (--top, (%1 = stack[top][0]), (%2 = stack[top][1]))
	
	static stack[256][2];
	new top = 0;
	
	if (size <= 1)
		return;
	
	new left, right, pivot, l_hold, r_hold;
	
	PUSH(0, size - 1);
	
	while (0 < top) {
		POP(left, right);
		
		pivot = numbers[left];
		l_hold = left;
		r_hold = right;

		if (right - left <= 6) {
			insertion_sort(numbers, left, right);

			continue;
		}

		while (left < right) {
			while (numbers[right] >= pivot && left < right)
				right--;

			if (left != right) {
				numbers[left] = numbers[right];
				left++;
			}

			while (numbers[left] <= pivot && left < right)
				left++;

			if (left != right) {
				numbers[right] = numbers[left];
				right--;
			}
		}

		numbers[left] = pivot;
		pivot = left;
		left = l_hold;
		right = r_hold;
		
		if (left < pivot)  PUSH(left, pivot - 1);
		if (right > pivot) PUSH(pivot + 1, right);
	}
	
	#undef PUSH
	#undef POP
}


main() {
	new numbers[] = {-496, -207, -353, -423, 174, 216, -346, 195, 184, -89, 488, -375, 473, -56, 302, 383, 12, -393, -186, -41, -197, 122, -201, 357, 60, 267, -499, 404, 247, 437, 320, 251, -270, 467, 329, 404, 183, 483, 99, -134, -107, 86, -9, -134, -470, -207, -252, 43, -99, 63, -499, 204, -316, -199, 61, 244, -432, 62, 148, 315, -1, -33, 66, 229, -66, -106, 133, -384, -124, -268, -17, 270,
	                 319, 474, -365, 349, -233, -117, -108, 168, 446, -107, -128, -370, 195, 433, 375, 263, -5, 22, 77, 494, 490, -358, 223, -77, 37, -144, 40, 413, 88, -477, 182, -93, 497, 318, -244, -236, -300, 148, 433, -354, -459, -196, -224, 236, -264, -350, -1, 232, 173, -425, 225, 162, -282, -52, -415, 256, 305, 126, 168, -107, 149, -150, 300, 146, -333, -445, 410, -132, 204, 342, 15, 245, -354, 291,
	                 -19, -117, 442, 480, -386, 115, -445, 340, -223, -226, -212, -137, -471, -408, 489, 197, -15, 138, -453, -216, -217, -285, -160, -307, 84, -457, -465, -402, 288, -319, 390, -232, 65, 331, -252, 179, -54, -197, 19, 223, 77, 307, -415, 106, 399, -426, -197, -117, 212, -149, 168, 496, 66, -493};
	new sorted[] = {-499, -499, -496, -493, -477, -471, -470, -465, -459, -457, -453, -445, -445, -432, -426, -425, -423, -415, -415, -408, -402, -393, -386, -384, -375, -370, -365, -358, -354, -354, -353, -350, -346, -333, -319, -316, -307, -300, -285, -282, -270, -268, -264, -252, -252, -244, -236, -233, -232, -226, -224, -223, -217, -216, -212, -207, -207, -201, -199, -197, -197, -197, -196,
	                -186, -160, -150, -149, -144, -137, -134, -134, -132, -128, -124, -117, -117, -117, -108, -107, -107, -107, -106, -99, -93, -89, -77, -66, -56, -54, -52, -41, -33, -19, -17, -15, -9, -5, -1, -1, 12, 15, 19, 22, 37, 40, 43, 60, 61, 62, 63, 65, 66, 66, 77, 77, 84, 86, 88, 99, 106, 115, 122, 126, 133, 138, 146, 148, 148, 149, 162, 168, 168, 168, 173, 174, 179, 182, 183, 184, 195, 195, 197,
	                204, 204, 212, 216, 223, 223, 225, 229, 232, 236, 244, 245, 247, 251, 256, 263, 267, 270, 288, 291, 300, 302, 305, 307, 315, 318, 319, 320, 329, 331, 340, 342, 349, 357, 375, 383, 390, 399, 404, 404, 410, 413, 433, 433, 437, 442, 446, 467, 473, 474, 480, 483, 488, 489, 490, 494, 496, 497};
	
	QuickSort(numbers);
	
	for (new i = 0; i < sizeof(numbers); i++) {
		if (numbers[i] != sorted[i]) {
			printf("1st pass: [%d] %d != %d", i, numbers[i], sorted[i]);
		}
	}
	
	new start = GetTickCount();
	
	for (new i = 0; i < 10000; i++) {
		new numbers_[] = {-496, -207, -353, -423, 174, 216, -346, 195, 184, -89, 488, -375, 473, -56, 302, 383, 12, -393, -186, -41, -197, 122, -201, 357, 60, 267, -499, 404, 247, 437, 320, 251, -270, 467, 329, 404, 183, 483, 99, -134, -107, 86, -9, -134, -470, -207, -252, 43, -99, 63, -499, 204, -316, -199, 61, 244, -432, 62, 148, 315, -1, -33, 66, 229, -66, -106, 133, -384, -124, -268, -17, 270,
		                  319, 474, -365, 349, -233, -117, -108, 168, 446, -107, -128, -370, 195, 433, 375, 263, -5, 22, 77, 494, 490, -358, 223, -77, 37, -144, 40, 413, 88, -477, 182, -93, 497, 318, -244, -236, -300, 148, 433, -354, -459, -196, -224, 236, -264, -350, -1, 232, 173, -425, 225, 162, -282, -52, -415, 256, 305, 126, 168, -107, 149, -150, 300, 146, -333, -445, 410, -132, 204, 342, 15};
		
		QuickSort(numbers_);
	}
	
	start = GetTickCount() - start;
	
	printf("took %dms (each iter: %.4fms)", start, floatdiv(start, 10000.0));
}