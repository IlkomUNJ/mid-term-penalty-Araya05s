#ifndef CUSTOMMATRIX_H
#define CUSTOMMATRIX_H

#include <iostream>

using namespace std;

class CustomMatrix{
public:
    bool mat[3][3]={0};
    int originx;
    int originy;
    CustomMatrix(){

    }

    //alternate way to instantiate the class
    CustomMatrix(bool m[3][3], int x, int y){
        const int m_size = sizeof(m) / sizeof(m[0]);
        for (int i=0;i< m_size;i++) {
            for(int j=0; j < sizeof(m[0]);j++){
                //cout << i << " " << j << endl;
                mat[i][j] = m[i][j];
                originx = x;
                originy = y;
            }
        }
    }

    void fillMatrix(bool m[3][3]){
        const int m_size = sizeof(m) / sizeof(m[0]);
        for (int i=0;i< m_size;i++) {
            for(int j=0; j < sizeof(m[0]);j++){
                //cout << i << " " << j << endl;
                mat[i][j] = m[i][j];
                originx = j;
                originx = i;
            }
        }
    }

    bool isAllFalse() const{
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (mat[i][j]) return false;
            }
        }
        return true;
    }

    bool isPatternMatch(bool m[3][3], bool p[3][3]){
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (mat[i][j] != p[i][i]) return false;
            }
        }
        return true;
    }
};

#endif // CUSTOMMATRIX_H
