#include <iostream>
#include <stdio.h>
#include "board.hpp"
#include "cell_evaluate.hpp"



#define inf 1000000000


int arr[64];
void print(){
    for(int i=0;i<hw;i++)
    {
        for(int j=0;j<hw;j++){
            if(arr[8*i+j]==black)
            printf("●");
            else if(arr[8*i+j]==white)
            printf("〇");
            else printf("・");
        }
        printf("\n");
    }
    return;
};


int main(){
    int kar;
    for(int i=0;i<hw2;i++)
    {
        arr[i] = vacant;
    }
arr[27]=white;
arr[28]=black;
arr[35]=black;
arr[36]=white;

print();
  

    return 0;
}
