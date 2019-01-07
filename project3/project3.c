#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>

void top_line (int, int, char*);
void bottom_line (int, int, char*);
void left_line (int, int, char*);
void right_line (int, int, char*);
void edge_line (int, int, char*);
void in_line (int, int, char*);

int main(int argc, char **argv)
{
    int nprocs, myrank;
    int **result;
    int b_size, generation, ghost; 
    int err = MPI_Init(&argc, &argv);

    if(err == MPI_SUCCESS) {
    } else {
        printf("MPI Failure\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    //printf("nprocs = %d myrank = %d\n", nprocs, myrank);
    if(myrank == 0) {
        int a, row = 0, col = 0;
        int info[3];
        int **map;
        int **fmap;
        char b;
        //info[0] : Board Size, info[1] : Generation, info[2] : Num of Ghost Cells.
        scanf("%d %d %d", &info[0], &info[1], &info[2]);
        if(info[1] < info[2])
            info[2] = info[1];
        scanf("%c", &b);
        b_size = info[0];
        generation = info[1];
        ghost = info[2];
        if(nprocs > 64)
            nprocs = 64;
        //proc_num : Num of totla process(sqrt)
        int proc_num = (int)sqrt(nprocs);
        int cnt = 0;
        int padding_num = 0;
        if(info[0]%proc_num != 0) {
            cnt = (info[0]/proc_num)+1;
            padding_num = cnt*proc_num-info[0]; 
        }

        info[0] += padding_num;
        //split : Num of block size in one process after padding.
        int split = (int)(info[0]/proc_num);
        //printf("%d %d %d %d\n",proc_num,cnt,padding_num,split);
        map = (int **)malloc(sizeof(int *)*info[0]);
        for(int i=0; i<info[0]; i++)
            map[i] = (int *)calloc(info[0], sizeof(int));

        for(int i=0; i<(info[0]+1)*(info[0]+1); i++) {
            scanf("%c", &b);
            if(b != '\n') {
                if(b == '#') map[row][col++] = 1;
                else if(b == '.') map[row][col++] = 0;
            } else {col = 0;row++;}
        }
        //printf("%d %d %d %d\n",split, info[0], info[1], info[2]);
        int send_info[5] = {split, info[0], info[1], info[2]+1, padding_num};
        for(int i=0;i<nprocs;i++) 
            MPI_Send(send_info, 4, MPI_INT, i, 1, MPI_COMM_WORLD);

        int block[split][split];
        int flag1;
        int flag2;
        for(int s=0; s<nprocs; s++) {
            flag1 = (s/proc_num)*split;
            flag2 = (s%proc_num)*split;
            for(int i=0;i<split;i++) 
                for(int j=0;j<split;j++){
                    block[i][j] = map[i+flag1][j+flag2];
                    //printf("[%d %d %d]\n", block[i][j], i+flag1, j+flag2);
                }
            MPI_Send(&block, split*split, MPI_INT, s, 1, MPI_COMM_WORLD);
        }

        result = (int **)malloc(sizeof(int *)*info[0]);
        for(int s=0; s<info[0]; s++)
            result[s] = (int *)calloc(info[0], sizeof(int));
    }

    //Each Process
    int my_info[5];
    MPI_Recv(my_info, 5, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    int my_block[my_info[0]][my_info[0]] = {0};
    MPI_Recv(&my_block, my_info[0]*my_info[0], MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    //if(myrank == 0) printf("[%d %d %d %d %d]\n", my_info[0], my_info[1], my_info[2], my_info[3], my_info[4]);
    //printf("%d's \n", myrank);for(int i=0; i<my_info[0]; i++) {for(int j=0; j<my_info[0]; j++) {if(my_block[i][j] == 1) printf("#"); else printf(".");}printf("\n");}
    //printf("--------------------------\n");
    int gen = my_info[3] + my_info[0];
    int newq = gen+my_info[3];
    int cal[newq][newq] = { 0, };
    for(int i=0;i<newq;i++) for(int j=0;j<newq;j++) cal[i][j]=0;
    for(int i=my_info[3]; i<gen; i++) {
        for(int j=my_info[3]; j<gen; j++) {                                    
            cal[i][j] = my_block[i-my_info[3]][j-my_info[3]];
        }
    }
    /*if(myrank == 1){
      for(int i=0;i<newq;i++) {
      for(int j=0;j<newq;j++) 
      printf("%d ",cal[i][j]);
      printf("\n");
      }
      }*/
    char r[8];

    int my_rank = myrank;
    int sproc = (int)sqrt(nprocs);
    if(nprocs == 4) 
    { edge_line(sproc, my_rank, r);
    } else if(nprocs == 9) {
        if(my_rank == 0 || my_rank == 2 || my_rank == 6 || my_rank == 8) edge_line(sproc, my_rank, r);        
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);
    } else if(nprocs == 16) {
        if(my_rank == 0 || my_rank == 3 || my_rank == 12 || my_rank == 15) edge_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);
    } else if(nprocs == 25) {
        if(my_rank == 0 || my_rank == 4 || my_rank == 20 || my_rank == 24) edge_line(sproc, my_rank, r);        
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);
    } else if(nprocs == 36) {
        if(my_rank == 0 || my_rank == 5 || my_rank == 30 || my_rank == 35) edge_line(sproc, my_rank, r);        
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);
    } else if(nprocs == 49) {
        if(my_rank == 0 || my_rank == 6 || my_rank == 42 || my_rank == 48) edge_line(sproc, my_rank, r);        
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);
    } else if(nprocs == 64) {
        if(my_rank == 0 || my_rank == 7 || my_rank == 56 || my_rank == 63) edge_line(sproc, my_rank, r);        
        else if((int)my_rank/sproc == 0) top_line(sproc, my_rank, r);
        else if(my_rank%sproc == 0) left_line(sproc, my_rank, r);
        else if(my_rank%sproc == sproc-1) right_line(sproc, my_rank, r);
        else if((int)my_rank/sproc == sproc-1) bottom_line(sproc, my_rank, r);
        else in_line(sproc, my_rank, r);        
    }

    //printf("%d's %d %d %d %d\n", my_rank, my_info[0], my_info[1], my_info[2], my_info[3]);  
    //int **from_t_l, **from_t, **from_t_r, **from_l, **from_r, **from_b_l, **from_b, **from_b_r;
    //int **send_t_l, **send_t, **send_t_r, **send_l, **send_r, **send_b_l, **send_b, **send_b_r;
    /*from_t_l = (int **)malloc(my_info[3]*sizeof(int*));
      from_t = (int **)malloc(my_info[3]*sizeof(int*));
      from_t_r = (int **)malloc(my_info[3]*sizeof(int*));
      from_l = (int **)malloc(my_info[0]*sizeof(int*));
      from_r = (int **)malloc(my_info[0]*sizeof(int*));
      from_b_l = (int **)malloc(my_info[3]*sizeof(int*));
      from_b = (int **)malloc(my_info[3]*sizeof(int*));
      from_b_r = (int **)malloc(my_info[3]*sizeof(int*));
     */    
    int from_t_l[my_info[3]][my_info[3]] = {0};
    int from_t[my_info[3]][my_info[0]] = {0};
    int from_t_r[my_info[3]][my_info[3]] = {0};
    int from_l[my_info[0]][my_info[3]] = {0};
    int from_r[my_info[0]][my_info[3]] = {0};
    int from_b_l[my_info[3]][my_info[3]] = {0};
    int from_b[my_info[3]][my_info[0]] = {0};
    int from_b_r[my_info[3]][my_info[3]] = {0};

    int send_t_l[my_info[3]][my_info[3]] = {0};
    int send_t[my_info[3]][my_info[0]] = {0};
    int send_t_r[my_info[3]][my_info[3]] = {0};
    int send_l[my_info[0]][my_info[3]] = {0};
    int send_r[my_info[0]][my_info[3]] = {0};
    int send_b_l[my_info[3]][my_info[3]] = {0};
    int send_b[my_info[3]][my_info[0]] = {0};
    int send_b_r[my_info[3]][my_info[3]] = {0};
    for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[3];j++) 
    {from_t_l[i][j]=0;from_t_r[i][j]=0;from_b_l[i][j]=0;from_b_r[i][j]=0;}
    for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[0];j++) {from_t[i][j]=0;from_b[i][j]=0;} 
    for(int i=0;i<my_info[0];i++) for(int j=0;j<my_info[3];j++) {from_l[i][j]=0;from_r[i][j]=0;}
    for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[3];j++) 
    {send_t_l[i][j]=0;send_t_r[i][j]=0;send_b_l[i][j]=0;send_b_r[i][j]=0;}
    for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[0];j++) {send_t[i][j]=0;send_b[i][j]=0;} 
    for(int i=0;i<my_info[0];i++) for(int j=0;j<my_info[3];j++) {send_l[i][j]=0;send_r[i][j]=0;}
    /*send_t_l = (int **)malloc(my_info[3]*sizeof(int*));
      send_t = (int **)malloc(my_info[3]*sizeof(int*));
      send_t_r = (int **)malloc(my_info[3]*sizeof(int*));
      send_l = (int **)malloc(my_info[0]*sizeof(int*));
      send_r = (int **)malloc(my_info[0]*sizeof(int*));
      send_b_l = (int **)malloc(my_info[3]*sizeof(int*));
      send_b = (int **)malloc(my_info[3]*sizeof(int*));
      send_b_r = (int **)malloc(my_info[3]*sizeof(int*));
     */
    //for(int i=0;i<my_info[3];i++) {
    //from_t_l[i]=(int *)calloc(my_info[3], sizeof(int));from_t_r[i]=(int *)calloc(my_info[3], sizeof(int));
    //from_b_l[i]=(int *)calloc(my_info[3], sizeof(int));from_b_r[i]=(int *)calloc(my_info[3], sizeof(int));
    //send_t_l[i]=(int *)calloc(my_info[3], sizeof(int));send_t_r[i]=(int *)calloc(my_info[3], sizeof(int));
    //send_b_l[i]=(int *)calloc(my_info[3], sizeof(int));send_b_r[i]=(int *)calloc(my_info[3], sizeof(int));
    //from_t[i]=(int *)calloc(my_info[0], sizeof(int));from_b[i] = (int *)calloc(my_info[0], sizeof(int));
    //send_t[i]=(int *)calloc(my_info[0], sizeof(int));send_b[i] = (int *)calloc(my_info[0], sizeof(int));
    //}
    //for(int i=0;i<my_info[0];i++) {
    //from_l[i] = (int *)calloc(my_info[3], sizeof(int));
    //from_r[i] = (int *)calloc(my_info[3], sizeof(int));
    //send_l[i] = (int *)calloc(my_info[3], sizeof(int));
    //send_r[i] = (int *)calloc(my_info[3], sizeof(int));
    //}
    int g;
    for(g=0; g<my_info[2]; g+=my_info[3]) 
    {
        for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[3];j++) 
        {from_t_l[i][j]=0;from_t_r[i][j]=0;from_b_l[i][j]=0;from_b_r[i][j]=0;}
        for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[0];j++) {from_t[i][j]=0;from_b[i][j]=0;} 
        for(int i=0;i<my_info[0];i++) for(int j=0;j<my_info[3];j++) {from_l[i][j]=0;from_r[i][j]=0;}
        for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[3];j++) 
        {send_t_l[i][j]=0;send_t_r[i][j]=0;send_b_l[i][j]=0;send_b_r[i][j]=0;}
        for(int i=0;i<my_info[3];i++) for(int j=0;j<my_info[0];j++) {send_t[i][j]=0;send_b[i][j]=0;} 
        for(int i=0;i<my_info[0];i++) for(int j=0;j<my_info[3];j++) {send_l[i][j]=0;send_r[i][j]=0;}
        if(g % my_info[3] == 0) 
        {
            int sen = my_info[0]-my_info[3];
            //if(my_rank == 0) printf("세대 : %d, 통신 수 : %d\n", g, my_info[3]);
            if(my_rank == 0 && nprocs != 1) 
            { //Top_Left Edge
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) 
                        send_r[q][k] = my_block[q][sen+k];
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        send_b[q][k] = my_block[sen+q][k];
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_b_r[q][k] = my_block[sen+q][sen+k];
                MPI_Send(send_b_r, my_info[3]*my_info[3], MPI_INT, r[7],1, MPI_COMM_WORLD);

            } else if(my_rank == sproc-1 && nprocs != 1) 
            { //Top_Right Edge
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_l[q][k] = my_block[q][k];
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++) 
                    for(int k=0;k<my_info[3];k++)
                        send_b_l[q][k]= my_block[sen+q][k];
                MPI_Send(send_b_l, my_info[3]*my_info[3], MPI_INT, r[5],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) 
                        send_b[q][k] = my_block[sen+q][k];
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6],1, MPI_COMM_WORLD);

            } else if(my_rank == sproc*(sproc-1) && nprocs != 1)
            { //Bottom_Left Edge
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        send_t[q][k] = my_block[q][k];
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_t_r[q][k] = my_block[q][sen+k];                
                MPI_Send(send_t_r, my_info[3]*my_info[3], MPI_INT, r[2],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_r[q][k] = my_block[q][sen+k];
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4],1, MPI_COMM_WORLD);

            } else if(my_rank == sproc*sproc-1 && nprocs != 1)
            { //Bottom_Right Edge
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_t_l[q][k] = my_block[q][k];                
                MPI_Send(send_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        send_t[q][k] = my_block[q][k];
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1],1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_l[q][k] = my_block[q][k];
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3],1, MPI_COMM_WORLD);

            } else if((int)my_rank/sproc == 0 && nprocs != 1)
            { //Top Line
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_l[q][k] = my_block[q][k];
                        send_r[q][k] = my_block[q][sen+k];
                    }
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD); 
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD);
                
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_b_l[q][k] = my_block[sen+q][k];
                        send_b_r[q][k] = my_block[sen+q][sen+k];
                    }
                MPI_Send(send_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD);
                MPI_Send(send_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        send_b[q][k] = my_block[sen+q][k];
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD);

            } else if(my_rank%sproc == 0 && nprocs != 1)
            { //Left Line
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        send_t[q][k] = my_block[q][k];
                        send_b[q][k] = my_block[sen+q][k];
                    }                
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD);
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_t_r[q][k] = my_block[q][sen+k];
                        send_b_r[q][k] = my_block[sen+q][sen+k];
                    }
                MPI_Send(send_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD);
                MPI_Send(send_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) 
                        send_r[q][k] = my_block[q][sen+k];
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD);                

            } else if(my_rank%sproc == sproc-1 && nprocs != 1)
            { //Right Line
                for(int q=0;q<my_info[3];q++) 
                    for(int k=0;k<my_info[3];k++) {
                        send_t_l[q][k] = my_block[q][k];
                        send_b_l[q][k] = my_block[sen+q][k];
                    }
                MPI_Send(send_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD);
                MPI_Send(send_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        send_t[q][k] = my_block[q][k];
                        send_b[q][k] = my_block[sen+q][k];
                    }
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD);
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        send_l[q][k] = my_block[q][k];
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD);

            } else if((int)myrank/sproc == sproc-1 && nprocs != 1)
            { //Bottom Line
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_t_l[q][k] = my_block[q][k];
                        send_t_r[q][k] = my_block[q][sen+k];
                    }
                MPI_Send(send_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD);
                MPI_Send(send_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD);                

                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        send_t[q][k] = my_block[q][k];  
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD);

                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_l[q][k] = my_block[q][k]; 
                        send_r[q][k] = my_block[q][sen+k];
                    }
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD);
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD);

            } else if(nprocs != 1)
            { //In Line
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_t_l[q][k] = my_block[q][k];
                        send_t_r[q][k] = my_block[q][sen+k];
                        send_b_l[q][k] = my_block[sen+q][k];
                        send_b_r[q][k] = my_block[sen+q][sen+k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        send_t[q][k] = my_block[q][k];
                        send_b[q][k] = my_block[sen+q][k];
                    }
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        send_l[q][k] = my_block[q][k];
                        send_r[q][k] = my_block[q][sen+k];
                    }

                MPI_Send(send_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD);
                MPI_Send(send_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD);
                MPI_Send(send_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD);
                MPI_Send(send_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD);
                MPI_Send(send_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD);
                MPI_Send(send_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD);
                MPI_Send(send_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD);
                MPI_Send(send_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD);
            }


            //receive
            if(my_rank == 0 && nprocs != 1)
            { //Top Left Edge
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[0];q++) {
                    for(int k=0;k<my_info[3];k++) {
                        cal[my_info[3]+q][gen+k] = from_r[q][k];
                        //printf("%d ",from_r[q][k]);
                    }
                    //printf("\n");
                }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[gen+q][my_info[3]+k] = from_b[q][k];
                for(int q=0;q<my_info[3];q++) {
                    for(int k=0;k<my_info[3];k++)
                        cal[gen+q][gen+k] = from_b_r[q][k];
                }    
            } else if(my_rank == sproc-1 && nprocs != 1)
            { //Top Right Edge
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[my_info[3]+q][k] = from_l[q][k];
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[gen+q][k] = from_b_l[q][k];
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[gen+q][my_info[3]+k] = from_b[q][k];

            } else if(my_rank == sproc*(sproc-1) && nprocs != 1)
            { //Bottom Left Edge
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[q][my_info[3]+k] = from_t[q][k];
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[q][gen+k] = from_t_r[q][k];
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[my_info[3]+q][gen+k] = from_r[q][k];

            } else if(my_rank == sproc*sproc-1 && nprocs != 1)
            { //Bottom Right Edge
                MPI_Recv(from_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[q][k] = from_t_l[q][k];
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[q][my_info[3]+k] = from_t[q][k];
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[my_info[3]+q][k] = from_l[q][k];

            } else if(my_rank/sproc == 0 && nprocs != 1)
            { //Top Line 
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[my_info[3]+q][k] = from_l[q][k];
                        cal[my_info[3]+q][gen+k] = from_r[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[gen+q][k] = from_b_l[q][k];
                        cal[gen+q][gen+k] = from_b_r[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[gen+q][my_info[3]+k] = from_b[q][k];

            } else if(my_rank%sproc == 0 && nprocs != 1)
            { //Left Line 
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        cal[q][my_info[3]+k] = from_t[q][k];
                        cal[gen+q][my_info[3]+k] = from_b[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[q][gen+k] = from_t_r[q][k];
                        cal[gen+q][gen+k] = from_b_r[q][k];
                    }
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[my_info[3]+q][gen+k] = from_r[q][k];

            } else if(my_rank%sproc == sproc-1 && nprocs != 1)
            { //Right Line                 
                MPI_Recv(from_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[q][k] = from_t_l[q][k];
                        cal[gen+q][k] = from_b_l[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        cal[q][my_info[3]+k] = from_t[q][k];
                        cal[gen+q][my_info[3]+k] = from_b[q][k];
                    }
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++)
                        cal[my_info[3]+q][k] = from_l[q][k];

            } else if(my_rank/sproc == sproc-1 && nprocs != 1)
            { //Bottom Line            
                MPI_Recv(from_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[q][k] = from_t_l[q][k];
                        cal[q][gen+k] = from_t_r[q][k];
                    }                
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[my_info[3]+q][k] = from_l[q][k];
                        cal[my_info[3]+q][gen+k] = from_r[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++)
                        cal[q][my_info[3]+k] = from_t[q][k];

            } else if(nprocs != 1)
            { //In Line                
                MPI_Recv(from_t_l, my_info[3]*my_info[3], MPI_INT, r[0], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t_r, my_info[3]*my_info[3], MPI_INT, r[2], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_l, my_info[3]*my_info[3], MPI_INT, r[5], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b_r, my_info[3]*my_info[3], MPI_INT, r[7], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_t, my_info[3]*my_info[0], MPI_INT, r[1], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_b, my_info[3]*my_info[0], MPI_INT, r[6], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_l, my_info[0]*my_info[3], MPI_INT, r[3], 1, MPI_COMM_WORLD, &status);
                MPI_Recv(from_r, my_info[0]*my_info[3], MPI_INT, r[4], 1, MPI_COMM_WORLD, &status);
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[q][k] = from_t_l[q][k];
                        cal[q][gen+k] = from_t_r[q][k];
                        cal[gen+q][k] = from_b_l[q][k];
                        cal[gen+q][gen+k] = from_b_r[q][k];
                    }
                for(int q=0;q<my_info[3];q++)
                    for(int k=0;k<my_info[0];k++) {
                        cal[q][my_info[3]+k] = from_t[q][k];
                        cal[gen+q][my_info[3]+k] = from_b[q][k];
                    }
                for(int q=0;q<my_info[0];q++)
                    for(int k=0;k<my_info[3];k++) {
                        cal[my_info[3]+q][k] = from_l[q][k];
                        cal[my_info[3]+q][gen+k] = from_r[q][k];
                    }
            }
            //for(int i=0;i<newq;i++){ if(i==0) printf("%d rank\n",my_rank);
            //for(int j=0;j<newq;j++){if(cal[i][j] == 1)printf("#");else printf(".");}printf("\n");}

            int sum = 0;
            int cnt = 0;
            int pivot = my_info[0]+my_info[3]*2;
            int newcal[newq][newq] = { 0, };
            for(int i=0;i<newq;i++) for(int j=0;j<newq;j++) newcal[i][j]=0;
            int plag = 0;
            for(int t=0;t<my_info[3];t++) 
            {
                cnt++;            
                //if(g+cnt == my_info[2]) break;
                //if(my_rank == 1) printf("%d세대 %d회pivot : %d\n",g,cnt,pivot);
                for(int row=cnt; row<pivot-cnt; row++)
                {
                    for(int col=cnt; col<pivot-cnt; col++) {
                        //if(my_rank == 0 ) printf("[%d, %d] ", row,col);
                        sum=cal[row-1][col-1]+cal[row-1][col]+cal[row-1][col+1];
                        sum+=cal[row][col-1]+cal[row][col+1];
                        sum+=cal[row+1][col-1]+cal[row+1][col]+cal[row+1][col+1];
                        if(cal[row][col]==1&&(sum==2||sum==3)) newcal[row][col]=1;
                        else if(cal[row][col]==1&&sum>3) newcal[row][col]=0;
                        else if(cal[row][col]==1&&sum<2) newcal[row][col]=0;
                        else if(cal[row][col]==0&&sum==3) newcal[row][col]=1;
                        else newcal[row][col]=0;
                        sum = 0;
                    }//if(my_rank==0)printf("\n");
                }
                for(int row=cnt; row<pivot-cnt; row++){
                    for(int col=cnt; col<pivot-cnt; col++){
                        cal[row][col] = newcal[row][col];
                        //if(my_rank == 0)
                        //printf("(%d) ",cal[row][col]);
                    }//if(my_rank == 0)printf("\n");
                }

                if(g+cnt == my_info[2]) {
                    //printf("g: %d /// %d에서종료\n",g, cnt);
                    plag = cnt; 
                    break;
                }
            }
            
            for(int q=my_info[3];q<my_info[3]+my_info[0];q++)
                for(int k=my_info[3];k<my_info[3]+my_info[0];k++)
                    my_block[q-my_info[3]][k-my_info[3]] = cal[q][k];
            
            if(my_rank == 0) 
            { //Top-Left
                for(int i=0;i<cnt;i++)
                    for(int j=0;j<newq;j++){
                        cal[i][j] = 0;
                        cal[j][i] = 0;
                    }            
                if(nprocs==1) {
                    for(int i=pivot-cnt;i<newq;i++) 
                        for(int j=0;j<newq;j++) {
                            cal[i][j] = 0;
                            cal[j][i] = 0;
                        }                    
                }
            }
            else if(my_rank == sproc-1) { //Top-Right
                for(int i=0;i<cnt;i++)
                    for(int j=0;j<newq;j++) 
                        cal[i][j] = 0;
                for(int i=0;i<newq;i++)
                    for(int j=pivot-cnt;j<newq;j++)
                        cal[i][j] = 0;
            }
            else if(my_rank == sproc*(sproc-1)) { //Bottom-Left
                for(int i=0;i<newq;i++)
                    for(int j=0;j<cnt-1;j++)
                        cal[i][j] = 0;
                for(int i=pivot-cnt;i<newq;i++)
                    for(int j=0;j<newq;j++)
                        cal[i][j] = 0;
            }
            else if(my_rank == sproc*sproc-1) { //Bottom-Right
                for(int i=pivot-cnt;i<newq;i++)
                    for(int j=0;j<newq;j++) {
                        cal[i][j] = 0;
                        cal[j][i] = 0;
                    }
            }
            else if(my_rank/sproc == 0) { //Top
                for(int i=0;i<cnt;i++)
                    for(int j=0;j<newq;j++)
                        cal[i][j]=0;
            }
            else if(my_rank/sproc == sproc-1) { //Bottom
                //for(int i=pivot-cnt;i<newq;i++)
               //     for(int j=0;j<newq;j++)
               //         cal[i][j]=0;
            }
            else if(my_rank%sproc == 0) { //Left
                for(int i=0;i<newq;i++)
                    for(int j=0;j<cnt;j++)
                        cal[i][j]=0;
            }
            else if(my_rank%sproc == sproc-1) { //Right 
                for(int i=0;i<newq;i++)
                    for(int j=pivot-cnt;j<newq;j++)
                        cal[i][j]=0;
            }

            if(g+plag == my_info[2]) 
            {
                if(my_rank == 0) {
                    int proc_num = (int)sqrt(nprocs);
                    for(int te=0; te<my_info[0]; te++)
                        for(int tm=0; tm<my_info[0]; tm++)
                            result[te][tm] = my_block[te][tm];

                    for(int ra = 1; ra < nprocs; ra++) {
                        int temp_b[my_info[0]][my_info[0]];
                        int fflag1 = (ra/proc_num)*my_info[0];
                        int fflag2 = (ra%proc_num)*my_info[0];
                        MPI_Recv(&temp_b, my_info[0]*my_info[0],MPI_INT, ra, 1, MPI_COMM_WORLD, &status);
                        //for(int i=0;i<my_info[0];i++) { if(i==0) printf("%d's index : [%d %d]\n",ra, fflag1, fflag2);
                        //    for(int j=0;j<my_info[0];j++) printf("%d ",temp_b[i][j]); printf("\n");}

                        for(int te = 0; te < my_info[0]; te++)
                            for(int tm = 0; tm < my_info[0]; tm++) 
                                result[te+fflag1][tm+fflag2] = temp_b[te][tm];

                    }
                    for(int i=0;i<my_info[1]-my_info[4];i++) {
                        for(int j=0;j<my_info[1]-my_info[4];j++) {
                            if(result[i][j] == 1)
                                printf("#");
                            else
                                printf(".");
                        }
                        printf("\n");
                    }                    
                } else {
                    //for(int i=0;i<my_info[0];i++) { if(i==0) printf("rank : %d\n", my_rank);
                    //    for(int j=0;j<my_info[0];j++) printf("[%d ]",my_block[i][j]); printf("\n");}
                    MPI_Send(&my_block, my_info[0]*my_info[0], MPI_INT, 0, 1, MPI_COMM_WORLD);
                }
            }
        } //End of iteration
    }
    MPI_Finalize();
    return 0;
}

void edge_line(int proc, int a, char *r){for(int i=0;i<8;i++)r[i] = -1;if(a == 0) {r[4] = a+1;r[6] = proc;r[7] = proc+1;} else if(a == proc-1) {r[3] = a-1;r[5] = a+proc-1;r[6] = a+proc;} else if(a == proc*proc-1) {r[0] = a-proc-1;r[1] = a-proc;r[3] = a-1;} else {r[1] = a-proc;r[2] = a-proc+1;r[4] = a+1;}}
void top_line(int proc, int a, char *r){for(int i=0;i<8;i++)r[i] = -1;r[3] = a-1;r[4] = a+1;r[5] = a+proc-1;r[6] = a+proc;r[7] = a+proc+1;}
void left_line(int proc, int a, char *r){for(int i=0;i<8;i++)r[i] = -1;r[1] = a-proc;r[2] = a-proc+1;r[4] = a+1;r[6] = a+proc;r[7] = a+proc+1;}
void right_line(int proc, int a, char *r){for(int i=0;i<8;i++) r[i] = -1;r[0] = a-proc-1;r[1] = a-proc;r[3] = a-1;r[5] = a+proc-1;r[6] = a+proc;}
void bottom_line(int proc, int a, char *r){for(int i=0;i<8;i++)r[i] = -1;r[0] = a-proc-1;r[1] = a-proc;r[2] = a-proc+1;r[3] = a-1;r[4] = a+1;}
void in_line(int proc, int a, char *r){r[0] = a-proc-1;r[1] = a-proc;r[2] = a-proc+1;r[3] = a-1;r[4] = a+1;r[5] = a+proc-1;r[6] = a+proc;r[7] = a+proc+1;}
