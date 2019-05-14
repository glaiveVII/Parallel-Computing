#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>


void print_Array (int* t, int elements) {
    for(int i = 0; i < elements; i++) {
        printf("%d\t", t[i]);
    }
}

void fusion(int a[], int l, int m, int r)
{

	int x = l;
    int y = m+1;
    int k = 0;

    int* t_tri = new int[(r - l)]; 
    while (x <= m &&  y <= r) { 
        if (a[x] < a[y]) {
            t_tri[k] = a[x];
            x++;
        }
        else {
            t_tri[k] = a[y];
            y++;
        }
        k++;
    }

    if (x <= m) { 
        while (x <= m) {
            t_tri[k] = a[x];
            x++;
            k++;
        }
    }
    else {
            while (y <= r) {
                t_tri[k] = a[y];
                y++;
                k++;
            }
    }

    for (int i = 0; i <= (r - l); i++)
        a[l + i] = t_tri[i];
}


void tri_fusion(int t[], int l, int r) {
    
    if (l != r) 
    {

    int m= (l + r) / 2; 
    #pragma omp parallel sections
    { 
       #pragma omp section
       tri_fusion(t, l, m);
       #pragma omp section
       tri_fusion(t, m + 1, r);
    }
    fusion(t, l, m, r);
    }
}
  


int main(void) {
    int elements = 10;
    int a[10] = { 10,2,1,5,6,3,4,9,8,7 };
    merge_sort(a, 0, elements-1);
    printArray(a, elements); // print sorted array
    printf("\n");

    printf("\n\n Tableau trié :");
    for (i = 0; i < n; i++) 
	printf(" %d", a[i]);
	printf("\n\n");
    return 0;
}


