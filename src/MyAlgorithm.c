#include "../include/MyAlgorithm.h"
/*
 * Thread function when block size = 2.
 */
void *myAlgorithmThreadB2(void *arg){
    myT_args *args = (myT_args*) arg;
    unsigned posX = args->beginPositionX, posY = args->beginPositionY;
    unsigned headPosinRes, feetPosinRes; // The corresponding index in the result array of the first and second row of the block.
    unsigned cols = args->matrix->cols, rows = args->matrix->rows;
    float **matrix = args->matrix->matrix;
    float *res = args->res->array;
    unsigned *idxArray = args->res->indexArray;
    // based on the matrix has been padded or not, divide the flow here to reduce
    // the total conditional branches in the loop
    if(args->padding){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 X
            // 1 A B
            // 3 C D
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            // load the block
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            for(unsigned j = 0; j < cols; j += 2){
                // loop has been unrolled to increase reusage
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec2[j] * vec3[j];
                varD += vec3[j] * vec4[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec2[j+1] * vec3[j+1];
                varD += vec3[j+1] * vec4[j+1];
            }
            // based on current i, j, caculate the corresponding index
            // in the result array of the first and second row of the block.
            headPosinRes = posInRes(idxArray, posY, posX);
            feetPosinRes = posInRes(idxArray, posY+1, posX);
            res[headPosinRes] = varA;
            // based on current position, filter out the unexpected value.
            if(posY != rows-2){
                if(posX != rows-2){
                    res[headPosinRes+1] = varB;
                    res[feetPosinRes+1] = varD;
                }
                if(posX != posY) res[feetPosinRes] = varC;
            }
            // jump to next block. If needed, jump to next row.
            posX += 2;
            if(posX >= rows){
                posY += 2;
                posX = posY;
            }
        }
    }else{
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 X
            // 1 A B
            // 3 C D
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            for(unsigned j = 0; j < cols; j += 2){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec2[j] * vec3[j];
                varD += vec3[j] * vec4[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec2[j+1] * vec3[j+1];
                varD += vec3[j+1] * vec4[j+1];
            }
            headPosinRes = posInRes(idxArray, posY, posX);
            feetPosinRes = posInRes(idxArray, posY+1, posX+1);
            res[headPosinRes] = varA;
            res[headPosinRes+1] = varB;
            res[feetPosinRes] = varD;
            if(posX!=posY) res[feetPosinRes-1] = varC;
            posX += 2;
            if(posX >= rows){
                posY += 2;
                posX = posY;
            }
        }
    }
    return NULL;
}

/*
 * Thread function when block size = 3.
 */
void *myAlgorithmThreadB3(void *arg){
    myT_args *args = (myT_args*) arg;
    unsigned posX = args->beginPositionX, posY = args->beginPositionY;
    unsigned headPos, bodyPos, feetPos; // There are 3 index here because addtional row is there due to block size 3
    float **matrix = args->matrix->matrix;
    unsigned cols = args->matrix->cols;
    unsigned rows = args->matrix->rows;
    float *res = args->res->array;
    unsigned *idxArray = args->res->indexArray;

    if(args->padding == 1){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 X
            // 1 A B C
            // 3 D E F
            // 5 G H I
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            for(unsigned j = 0; j < cols; j += 3){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec3[j] * vec2[j];
                varE += vec3[j] * vec4[j];
                varF += vec3[j] * vec6[j];
                varG += vec5[j] * vec2[j];
                varH += vec5[j] * vec4[j];
                varI += vec5[j] * vec6[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec3[j+1] * vec2[j+1];
                varE += vec3[j+1] * vec4[j+1];
                varF += vec3[j+1] * vec6[j+1];
                varG += vec5[j+1] * vec2[j+1];
                varH += vec5[j+1] * vec4[j+1];
                varI += vec5[j+1] * vec6[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec3[j+2] * vec2[j+2];
                varE += vec3[j+2] * vec4[j+2];
                varF += vec3[j+2] * vec6[j+2];
                varG += vec5[j+2] * vec2[j+2];
                varH += vec5[j+2] * vec4[j+2];
                varI += vec5[j+2] * vec6[j+2];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            feetPos = posInRes(idxArray, posY+2, posX);
            res[headPos] = varA;
            res[headPos+1] = varB;
            res[bodyPos+1] = varE;
            if(posX < rows - 3){
                res[headPos+2] = varC;
                res[bodyPos+2] = varF;
                res[feetPos+2] = varI;
            }
            if(posX != posY){
                res[bodyPos] = varD;
                res[feetPos+1] = varH;
                if(posX - posY > 1)
                    res[feetPos] = varG;
            }
            posX += 3;
            if(posX >= rows){
                posY += 3;
                posX = posY;
            }
        }
    }else if(args->padding == 2){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 X
            // 1 A B C
            // 3 D E F
            // 5 G H I
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            for(unsigned j = 0; j < cols; j += 3){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec3[j] * vec2[j];
                varE += vec3[j] * vec4[j];
                varF += vec3[j] * vec6[j];
                varG += vec5[j] * vec2[j];
                varH += vec5[j] * vec4[j];
                varI += vec5[j] * vec6[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec3[j+1] * vec2[j+1];
                varE += vec3[j+1] * vec4[j+1];
                varF += vec3[j+1] * vec6[j+1];
                varG += vec5[j+1] * vec2[j+1];
                varH += vec5[j+1] * vec4[j+1];
                varI += vec5[j+1] * vec6[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec3[j+2] * vec2[j+2];
                varE += vec3[j+2] * vec4[j+2];
                varF += vec3[j+2] * vec6[j+2];
                varG += vec5[j+2] * vec2[j+2];
                varH += vec5[j+2] * vec4[j+2];
                varI += vec5[j+2] * vec6[j+2];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            feetPos = posInRes(idxArray, posY+2, posX);

            res[headPos] = varA;
            if(posX < rows - 3){
                res[headPos+1] = varB;
                res[headPos+2] = varC;
                res[bodyPos+1] = varE;
                res[bodyPos+2] = varF;
                res[feetPos+2] = varI;
                if(posX != posY)res[feetPos+1] = varH;
            }
            if(posX != posY){
                res[bodyPos] = varD;
                res[feetPos] = varG;
            }
            posX += 3;
            if(posX >= rows){
                posY += 3;
                posX = posY;
            }
        }
    }else{
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 X
            // 1 A B C
            // 3 D E F
            // 5 G H I
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            for(unsigned j = 0; j < cols; j += 3){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec3[j] * vec2[j];
                varE += vec3[j] * vec4[j];
                varF += vec3[j] * vec6[j];
                varG += vec5[j] * vec2[j];
                varH += vec5[j] * vec4[j];
                varI += vec5[j] * vec6[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec3[j+1] * vec2[j+1];
                varE += vec3[j+1] * vec4[j+1];
                varF += vec3[j+1] * vec6[j+1];
                varG += vec5[j+1] * vec2[j+1];
                varH += vec5[j+1] * vec4[j+1];
                varI += vec5[j+1] * vec6[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec3[j+2] * vec2[j+2];
                varE += vec3[j+2] * vec4[j+2];
                varF += vec3[j+2] * vec6[j+2];
                varG += vec5[j+2] * vec2[j+2];
                varH += vec5[j+2] * vec4[j+2];
                varI += vec5[j+2] * vec6[j+2];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            feetPos = posInRes(idxArray, posY+2, posX);

            res[headPos] = varA;
            res[headPos+1] = varB;
            res[headPos+2] = varC;
            res[bodyPos+1] = varE;
            res[bodyPos+2] = varF;
            res[feetPos+2] = varI;
            if(posX!=posY){
                res[bodyPos] = varD;
                res[feetPos] = varG;
                res[feetPos+1] = varH;
            }
            posX += 3;
            if(posX >= rows){
                posY += 3;
                posX = posY;
            }
        }
    }
    return NULL;
}

/*
 * Thread function when block size = 4.
 */
void *myAlgorithmThreadB4(void *arg){
    myT_args *args = (myT_args*) arg;
    unsigned posX = args->beginPositionX, posY = args->beginPositionY;
    unsigned headPos, bodyPos, legPos, feetPos;
    float **matrix = args->matrix->matrix;
    unsigned cols = args->matrix->cols;
    unsigned rows = args->matrix->rows;
    float *res = args->res->array;
    unsigned *idxArray = args->res->indexArray;

    if(args->padding == 1){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 8 X
            // 1 A B C D
            // 3 E F G H
            // 5 I J K L
            // 7 M N O P
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float *vec7 = matrix[posY+3];
            float *vec8 = matrix[posX+3];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            float varJ = 0;
            float varK = 0;
            float varL = 0;
            float varM = 0;
            float varN = 0;
            float varO = 0;
            float varP = 0;
            for(unsigned j = 0; j < cols; j += 4){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec1[j] * vec8[j];
                varE += vec3[j] * vec2[j];
                varF += vec3[j] * vec4[j];
                varG += vec3[j] * vec6[j];
                varH += vec3[j] * vec8[j];
                varI += vec5[j] * vec2[j];
                varJ += vec5[j] * vec4[j];
                varK += vec5[j] * vec6[j];
                varL += vec5[j] * vec8[j];
                varM += vec7[j] * vec2[j];
                varN += vec7[j] * vec4[j];
                varO += vec7[j] * vec6[j];
                varP += vec7[j] * vec8[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec1[j+1] * vec8[j+1];
                varE += vec3[j+1] * vec2[j+1];
                varF += vec3[j+1] * vec4[j+1];
                varG += vec3[j+1] * vec6[j+1];
                varH += vec3[j+1] * vec8[j+1];
                varI += vec5[j+1] * vec2[j+1];
                varJ += vec5[j+1] * vec4[j+1];
                varK += vec5[j+1] * vec6[j+1];
                varL += vec5[j+1] * vec8[j+1];
                varM += vec7[j+1] * vec2[j+1];
                varN += vec7[j+1] * vec4[j+1];
                varO += vec7[j+1] * vec6[j+1];
                varP += vec7[j+1] * vec8[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec1[j+2] * vec8[j+2];
                varE += vec3[j+2] * vec2[j+2];
                varF += vec3[j+2] * vec4[j+2];
                varG += vec3[j+2] * vec6[j+2];
                varH += vec3[j+2] * vec8[j+2];
                varI += vec5[j+2] * vec2[j+2];
                varJ += vec5[j+2] * vec4[j+2];
                varK += vec5[j+2] * vec6[j+2];
                varL += vec5[j+2] * vec8[j+2];
                varM += vec7[j+2] * vec2[j+2];
                varN += vec7[j+2] * vec4[j+2];
                varO += vec7[j+2] * vec6[j+2];
                varP += vec7[j+2] * vec8[j+2];

                varA += vec1[j+3] * vec2[j+3];
                varB += vec1[j+3] * vec4[j+3];
                varC += vec1[j+3] * vec6[j+3];
                varD += vec1[j+3] * vec8[j+3];
                varE += vec3[j+3] * vec2[j+3];
                varF += vec3[j+3] * vec4[j+3];
                varG += vec3[j+3] * vec6[j+3];
                varH += vec3[j+3] * vec8[j+3];
                varI += vec5[j+3] * vec2[j+3];
                varJ += vec5[j+3] * vec4[j+3];
                varK += vec5[j+3] * vec6[j+3];
                varL += vec5[j+3] * vec8[j+3];
                varM += vec7[j+3] * vec2[j+3];
                varN += vec7[j+3] * vec4[j+3];
                varO += vec7[j+3] * vec6[j+3];
                varP += vec7[j+3] * vec8[j+3];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            legPos = posInRes(idxArray, posY+2, posX);
            feetPos = posInRes(idxArray, posY+3, posX);
            // padding = 1
            res[headPos] = varA;
            res[headPos+1] = varB;
            res[headPos+2] = varC;
            res[bodyPos+1] = varF;
            res[bodyPos+2] = varG;
            res[legPos+2] = varK;
            if(posX < rows - 4){
                res[headPos+3] = varD;
                res[bodyPos+3] = varH;
                res[legPos+3] = varL;
                res[feetPos+3] = varP;
            }
            if(posX != posY){
                res[bodyPos] = varE;
                res[legPos] = varI;
                res[legPos+1] = varJ;
                res[feetPos] = varM;
                res[feetPos+1] = varN;
                res[feetPos+2] = varO;
            }
            posX += 4;
            if(posX >= rows){
                posY += 4;
                posX = posY;
            }
        }
    }else if(args->padding == 2){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 8 X
            // 1 A B C D
            // 3 E F G H
            // 5 I J K L
            // 7 M N O P
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float *vec7 = matrix[posY+3];
            float *vec8 = matrix[posX+3];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            float varJ = 0;
            float varK = 0;
            float varL = 0;
            float varM = 0;
            float varN = 0;
            float varO = 0;
            float varP = 0;
            for(unsigned j = 0; j < cols; j += 4){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec1[j] * vec8[j];
                varE += vec3[j] * vec2[j];
                varF += vec3[j] * vec4[j];
                varG += vec3[j] * vec6[j];
                varH += vec3[j] * vec8[j];
                varI += vec5[j] * vec2[j];
                varJ += vec5[j] * vec4[j];
                varK += vec5[j] * vec6[j];
                varL += vec5[j] * vec8[j];
                varM += vec7[j] * vec2[j];
                varN += vec7[j] * vec4[j];
                varO += vec7[j] * vec6[j];
                varP += vec7[j] * vec8[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec1[j+1] * vec8[j+1];
                varE += vec3[j+1] * vec2[j+1];
                varF += vec3[j+1] * vec4[j+1];
                varG += vec3[j+1] * vec6[j+1];
                varH += vec3[j+1] * vec8[j+1];
                varI += vec5[j+1] * vec2[j+1];
                varJ += vec5[j+1] * vec4[j+1];
                varK += vec5[j+1] * vec6[j+1];
                varL += vec5[j+1] * vec8[j+1];
                varM += vec7[j+1] * vec2[j+1];
                varN += vec7[j+1] * vec4[j+1];
                varO += vec7[j+1] * vec6[j+1];
                varP += vec7[j+1] * vec8[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec1[j+2] * vec8[j+2];
                varE += vec3[j+2] * vec2[j+2];
                varF += vec3[j+2] * vec4[j+2];
                varG += vec3[j+2] * vec6[j+2];
                varH += vec3[j+2] * vec8[j+2];
                varI += vec5[j+2] * vec2[j+2];
                varJ += vec5[j+2] * vec4[j+2];
                varK += vec5[j+2] * vec6[j+2];
                varL += vec5[j+2] * vec8[j+2];
                varM += vec7[j+2] * vec2[j+2];
                varN += vec7[j+2] * vec4[j+2];
                varO += vec7[j+2] * vec6[j+2];
                varP += vec7[j+2] * vec8[j+2];

                varA += vec1[j+3] * vec2[j+3];
                varB += vec1[j+3] * vec4[j+3];
                varC += vec1[j+3] * vec6[j+3];
                varD += vec1[j+3] * vec8[j+3];
                varE += vec3[j+3] * vec2[j+3];
                varF += vec3[j+3] * vec4[j+3];
                varG += vec3[j+3] * vec6[j+3];
                varH += vec3[j+3] * vec8[j+3];
                varI += vec5[j+3] * vec2[j+3];
                varJ += vec5[j+3] * vec4[j+3];
                varK += vec5[j+3] * vec6[j+3];
                varL += vec5[j+3] * vec8[j+3];
                varM += vec7[j+3] * vec2[j+3];
                varN += vec7[j+3] * vec4[j+3];
                varO += vec7[j+3] * vec6[j+3];
                varP += vec7[j+3] * vec8[j+3];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            legPos = posInRes(idxArray, posY+2, posX);
            feetPos = posInRes(idxArray, posY+3, posX);
            //padding = 2
            res[headPos] = varA;
            res[headPos+1] = varB;
            res[bodyPos+1] = varF;
            if(posX < rows - 4){
                res[headPos+2] = varC;
                res[headPos+3] = varD;
                res[bodyPos+2] = varG;
                res[bodyPos+3] = varH;
                res[legPos+2] = varK;
                res[legPos+3] = varL;
                res[feetPos+3] = varP;
                if(posX!=posY) res[feetPos+2] = varO;
            }
            if(posX!=posY){
                res[bodyPos] = varE;
                res[legPos] = varI;
                res[legPos+1] = varJ;
                res[feetPos] = varM;
                res[feetPos+1] = varN;
            }
            posX += 4;
            if(posX >= rows){
                posY += 4;
                posX = posY;
            }
        }
    }else if(args->padding == 3){
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 8 X
            // 1 A B C D
            // 3 E F G H
            // 5 I J K L
            // 7 M N O P
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float *vec7 = matrix[posY+3];
            float *vec8 = matrix[posX+3];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            float varJ = 0;
            float varK = 0;
            float varL = 0;
            float varM = 0;
            float varN = 0;
            float varO = 0;
            float varP = 0;
            for(unsigned j = 0; j < cols; j += 4){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec1[j] * vec8[j];
                varE += vec3[j] * vec2[j];
                varF += vec3[j] * vec4[j];
                varG += vec3[j] * vec6[j];
                varH += vec3[j] * vec8[j];
                varI += vec5[j] * vec2[j];
                varJ += vec5[j] * vec4[j];
                varK += vec5[j] * vec6[j];
                varL += vec5[j] * vec8[j];
                varM += vec7[j] * vec2[j];
                varN += vec7[j] * vec4[j];
                varO += vec7[j] * vec6[j];
                varP += vec7[j] * vec8[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec1[j+1] * vec8[j+1];
                varE += vec3[j+1] * vec2[j+1];
                varF += vec3[j+1] * vec4[j+1];
                varG += vec3[j+1] * vec6[j+1];
                varH += vec3[j+1] * vec8[j+1];
                varI += vec5[j+1] * vec2[j+1];
                varJ += vec5[j+1] * vec4[j+1];
                varK += vec5[j+1] * vec6[j+1];
                varL += vec5[j+1] * vec8[j+1];
                varM += vec7[j+1] * vec2[j+1];
                varN += vec7[j+1] * vec4[j+1];
                varO += vec7[j+1] * vec6[j+1];
                varP += vec7[j+1] * vec8[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec1[j+2] * vec8[j+2];
                varE += vec3[j+2] * vec2[j+2];
                varF += vec3[j+2] * vec4[j+2];
                varG += vec3[j+2] * vec6[j+2];
                varH += vec3[j+2] * vec8[j+2];
                varI += vec5[j+2] * vec2[j+2];
                varJ += vec5[j+2] * vec4[j+2];
                varK += vec5[j+2] * vec6[j+2];
                varL += vec5[j+2] * vec8[j+2];
                varM += vec7[j+2] * vec2[j+2];
                varN += vec7[j+2] * vec4[j+2];
                varO += vec7[j+2] * vec6[j+2];
                varP += vec7[j+2] * vec8[j+2];

                varA += vec1[j+3] * vec2[j+3];
                varB += vec1[j+3] * vec4[j+3];
                varC += vec1[j+3] * vec6[j+3];
                varD += vec1[j+3] * vec8[j+3];
                varE += vec3[j+3] * vec2[j+3];
                varF += vec3[j+3] * vec4[j+3];
                varG += vec3[j+3] * vec6[j+3];
                varH += vec3[j+3] * vec8[j+3];
                varI += vec5[j+3] * vec2[j+3];
                varJ += vec5[j+3] * vec4[j+3];
                varK += vec5[j+3] * vec6[j+3];
                varL += vec5[j+3] * vec8[j+3];
                varM += vec7[j+3] * vec2[j+3];
                varN += vec7[j+3] * vec4[j+3];
                varO += vec7[j+3] * vec6[j+3];
                varP += vec7[j+3] * vec8[j+3];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            legPos = posInRes(idxArray, posY+2, posX);
            feetPos = posInRes(idxArray, posY+3, posX);
            //padding = 3
            res[headPos] = varA;
            if(posX < rows - 4){
                res[headPos+1] = varB;
                res[headPos+2] = varC;
                res[headPos+3] = varD;
                res[bodyPos+1] = varF;
                res[bodyPos+2] = varG;
                res[bodyPos+3] = varH;
                res[legPos+2] = varK;
                res[legPos+3] = varL;
                res[feetPos+3] = varP;
                if(posX != posY){
                    res[legPos+1] = varJ;
                    res[feetPos+1] = varN;
                    res[feetPos+2] = varO;
                }
            }
            if(posX!=posY){
                res[bodyPos] = varE;
                res[legPos] = varI;
                res[feetPos] = varM;
            }
            posX += 4;
            if(posX >= rows){
                posY += 4;
                posX = posY;
            }
        }
    }else{
        for(unsigned i = 0; i < args->workLoad; ++i){
            //   2 4 6 8 X
            // 1 A B C D
            // 3 E F G H
            // 5 I J K L
            // 7 M N O P
            // Y
            float *vec1 = matrix[posY];
            float *vec2 = matrix[posX];
            float *vec3 = matrix[posY+1];
            float *vec4 = matrix[posX+1];
            float *vec5 = matrix[posY+2];
            float *vec6 = matrix[posX+2];
            float *vec7 = matrix[posY+3];
            float *vec8 = matrix[posX+3];
            float varA = 0;
            float varB = 0;
            float varC = 0;
            float varD = 0;
            float varE = 0;
            float varF = 0;
            float varG = 0;
            float varH = 0;
            float varI = 0;
            float varJ = 0;
            float varK = 0;
            float varL = 0;
            float varM = 0;
            float varN = 0;
            float varO = 0;
            float varP = 0;
            for(unsigned j = 0; j < cols; j += 4){
                varA += vec1[j] * vec2[j];
                varB += vec1[j] * vec4[j];
                varC += vec1[j] * vec6[j];
                varD += vec1[j] * vec8[j];
                varE += vec3[j] * vec2[j];
                varF += vec3[j] * vec4[j];
                varG += vec3[j] * vec6[j];
                varH += vec3[j] * vec8[j];
                varI += vec5[j] * vec2[j];
                varJ += vec5[j] * vec4[j];
                varK += vec5[j] * vec6[j];
                varL += vec5[j] * vec8[j];
                varM += vec7[j] * vec2[j];
                varN += vec7[j] * vec4[j];
                varO += vec7[j] * vec6[j];
                varP += vec7[j] * vec8[j];

                varA += vec1[j+1] * vec2[j+1];
                varB += vec1[j+1] * vec4[j+1];
                varC += vec1[j+1] * vec6[j+1];
                varD += vec1[j+1] * vec8[j+1];
                varE += vec3[j+1] * vec2[j+1];
                varF += vec3[j+1] * vec4[j+1];
                varG += vec3[j+1] * vec6[j+1];
                varH += vec3[j+1] * vec8[j+1];
                varI += vec5[j+1] * vec2[j+1];
                varJ += vec5[j+1] * vec4[j+1];
                varK += vec5[j+1] * vec6[j+1];
                varL += vec5[j+1] * vec8[j+1];
                varM += vec7[j+1] * vec2[j+1];
                varN += vec7[j+1] * vec4[j+1];
                varO += vec7[j+1] * vec6[j+1];
                varP += vec7[j+1] * vec8[j+1];

                varA += vec1[j+2] * vec2[j+2];
                varB += vec1[j+2] * vec4[j+2];
                varC += vec1[j+2] * vec6[j+2];
                varD += vec1[j+2] * vec8[j+2];
                varE += vec3[j+2] * vec2[j+2];
                varF += vec3[j+2] * vec4[j+2];
                varG += vec3[j+2] * vec6[j+2];
                varH += vec3[j+2] * vec8[j+2];
                varI += vec5[j+2] * vec2[j+2];
                varJ += vec5[j+2] * vec4[j+2];
                varK += vec5[j+2] * vec6[j+2];
                varL += vec5[j+2] * vec8[j+2];
                varM += vec7[j+2] * vec2[j+2];
                varN += vec7[j+2] * vec4[j+2];
                varO += vec7[j+2] * vec6[j+2];
                varP += vec7[j+2] * vec8[j+2];

                varA += vec1[j+3] * vec2[j+3];
                varB += vec1[j+3] * vec4[j+3];
                varC += vec1[j+3] * vec6[j+3];
                varD += vec1[j+3] * vec8[j+3];
                varE += vec3[j+3] * vec2[j+3];
                varF += vec3[j+3] * vec4[j+3];
                varG += vec3[j+3] * vec6[j+3];
                varH += vec3[j+3] * vec8[j+3];
                varI += vec5[j+3] * vec2[j+3];
                varJ += vec5[j+3] * vec4[j+3];
                varK += vec5[j+3] * vec6[j+3];
                varL += vec5[j+3] * vec8[j+3];
                varM += vec7[j+3] * vec2[j+3];
                varN += vec7[j+3] * vec4[j+3];
                varO += vec7[j+3] * vec6[j+3];
                varP += vec7[j+3] * vec8[j+3];
            }
            headPos = posInRes(idxArray, posY, posX);
            bodyPos = posInRes(idxArray, posY+1, posX);
            legPos = posInRes(idxArray, posY+2, posX);
            feetPos = posInRes(idxArray, posY+3, posX);
            //padding = 0
            res[headPos] = varA;
            res[headPos+1] = varB;
            res[headPos+2] = varC;
            res[headPos+3] = varD;
            res[bodyPos+1] = varF;
            res[bodyPos+2] = varG;
            res[bodyPos+3] = varH;
            res[legPos+2] = varK;
            res[legPos+3] = varL;
            res[feetPos+3] = varP;
            if(posX!=posY){
                res[bodyPos] = varE;
                res[legPos] = varI;
                res[legPos+1] = varJ;
                res[feetPos] = varM;
                res[feetPos+1] = varN;
                res[feetPos+2] = varO;
            }
            posX += 4;
            if(posX >= rows){
                posY += 4;
                posX = posY;
            }
        }
    }
    return NULL;
}

/*
 * Pad the matrix if needed.
 */
unsigned padding(MATRIX *matrix, unsigned b){
    unsigned remainder = matrix->rows % b; // Calculate the remainder where cannot be evenly distributed to each thread.
    unsigned paddedRows = 0;
    if(remainder != 0){
        paddedRows = b - remainder; // rows need to be added
        matrix->matrix = realloc(matrix->matrix, sizeof(float*) * (matrix->rows + paddedRows));
        for(unsigned i = matrix->rows; i < matrix->rows + paddedRows; ++i){
            matrix->matrix[i] = calloc(matrix->cols, sizeof(float));
        }
        matrix->rows += paddedRows;
    }
    // For loop-unrolling, each rows in the source matrix may need also to be padded
    remainder = matrix->cols % b;
    if(remainder != 0){
        unsigned pad = b - remainder;
        for(unsigned i = 0; i < matrix->rows; ++i){
            matrix->matrix[i] = realloc(matrix->matrix[i], (matrix->cols + pad) * sizeof(float));
            memset(&matrix->matrix[i][matrix->cols], 0, pad*sizeof(float));
        }
        matrix->cols += pad;
    }
    return paddedRows; // Return how many rows has been added into the source matrix.
}

/*
 * Designed algorithm.
 */
unsigned long myAlgorithm(MATRIX *matrix, RES *res, unsigned t, const unsigned b){
    TIME before, after;

    gettimeofday(&before, NULL);

    unsigned pad = padding(matrix, b); // get the matrix padded

    unsigned blocks = PAIR_NUM(matrix->rows / b); // Calculate the total workload.
    // Compute the work equally to each thread, and the remainder.
    unsigned equalWorkLoad = blocks/t, unequalWorkLoad = blocks % t;
    // If the workload is too small to make every thread works, then decrease the thread number.
    t = equalWorkLoad == 0 ? unequalWorkLoad:t;

    // Initialize the thread arguments.
    myT_args* threadArguments = malloc(sizeof(myT_args) * t);

    unsigned vecX = 0, vecY = 0;

    for(unsigned i = 0; i < t; ++i){
        threadArguments[i].matrix = matrix;
        threadArguments[i].res = res;
        threadArguments[i].beginPositionX = vecX;
        threadArguments[i].beginPositionY = vecY;
        threadArguments[i].workLoad = i < unequalWorkLoad ? equalWorkLoad + 1 : equalWorkLoad;
        threadArguments[i].padding = pad;
        if(i < t-1){ // caculate the begin position for next thread
            vecX += threadArguments[i].workLoad * b;
            while(vecX >= matrix->rows){
                vecY += b;
                vecX -= matrix->rows - vecY;
            }
        }
    }

    void*(*tFunc)(void*) = NULL;

    switch(b){
        case 3:
            tFunc = myAlgorithmThreadB3;
            break;
        case 4:
            tFunc = myAlgorithmThreadB4;
            break;
        case 2:
        default:
            tFunc = myAlgorithmThreadB2;
    }
    pthread_t *threads = malloc(sizeof(pthread_t) * t);


    for(unsigned i = 0; i < t; ++i){
        pthread_create(&threads[i], NULL, tFunc, &threadArguments[i]);
    }

    for(unsigned i = 0; i < t; ++i){
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&after, NULL);

    free(threads);
    free(threadArguments);

    return timediff(before, after);
}
