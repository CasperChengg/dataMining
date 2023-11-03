#include"./header_files/quick_sort.h"
void swap(float *a, float *b)
{
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;
}
void quickSort(float* x, int front, int rear)
{
    int mid = front + (rear - front) / 2;
    int frontBackUp = front, rearBackUp = rear;
    if(front >= rear)
    {
        return;
    }
    while(front <= rear)
    {
        while(front == mid || x[front] < x[mid])
        { 
            front++;
            if(front > rear)
            {
                break;
            }
        }
        while(rear == mid || x[rear] >= x[mid])
        {
            rear--;
        }
        if(front < rear){
            swap(&x[front], &x[rear]);
            front++;
            rear--;
        }
    }
    if(front > mid)
    {
        swap(&x[front - 1], &x[mid]); 
        quickSort(x, frontBackUp, front - 2);
        quickSort(x, front, rearBackUp);
    }
    else{
        swap(&x[front], &x[mid]); 
        quickSort(x, frontBackUp, front - 1);
        quickSort(x, front + 1, rearBackUp);
    }
}