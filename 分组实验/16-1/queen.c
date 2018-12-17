#include <mpi.h>
#include <stdio.h>

#define QUEENS 8
#define MAX_SOLUTIONS 92

typedef int bool;
const int true = 1;
const int false = 0;

/*ö���ź�*/
enum msg_content
{
    READY,
    ACCOMPLISHED,
    NEW_TASK,
    TERMINATE
};

/*ö����Ϣ��ǩ*/
enum msg_tag
{
    REQUEST_TAG,
    SEED_TAG,
    REPLY_TAG,
    NUM_SOLUTIONS_TAG,
    SOLUTIONS_TAG
};

int solutions[MAX_SOLUTIONS][QUEENS];
int solution_count = 0;

/*
 *��������collides
 *���ܣ��������λ���Ƿ����С��Խ��߻򷴶Խ��߳�ͻ
 *���룺row1Ϊλ��1���кţ�
 *      col1Ϊλ��1���кţ�
 *      row2Ϊλ��2���кţ�
 *      col2Ϊλ��2���кš�
 *���������1������λ�����С��Խ��߻򷴶Խ��߳�ͻ��
 *      ���򷵻�0
 */
bool collides(int row1, int col1, int row2, int col2)
{
    return (col1 == col2)
        || (col1 - col2 == row1 - row2)
        || (col1 + row1 == col2 + row2);
}                                                 /* collides */


/*
 *��������generate_seed
 *���ܣ����ɳ�ʼ�����̣�ֻ��ʼ�����̵�ǰ����
 *���룺��
 *���������0������û�пɳ�ʼ�������̣�
 *      ���򷵻����ɵĳ�ʼ�������̡�
 */
int generate_seed()
{
    static int seed = 0;

    do
    {
        seed++;
    } while (seed <= QUEENS * QUEENS - 1
        && collides(0, seed / QUEENS, 1, seed % QUEENS));

    if (seed > QUEENS * QUEENS - 1)
        return 0;
    else
        return seed;
}                                                 /* generate_seed */


/*
 *��������print_solutions
 *���ܣ���ӡ���
 *���룺countΪ��Ҫ��ӡ�Ľ���ĸ�����
 *      solutions[][QUEENS]Ϊ��Ҫ��ӡ�����Ľ�������ʺ����ڷŵ�λ�á�
 *�������
 */
void print_solutions(int count, int solutions[][QUEENS])
{
    int i, j, k;

    for (i = 0; i < count; i++)
    {
		/*��ӡ��i+1�����*/
        printf("Solution %d :\n", i + 1);
        for (j = 0; j < QUEENS; j++)
        {
            printf("%d ", solutions[i][j]);
			/*��ӡ���̣�*��ʾ�ʺ�����λ��*/
            for (k = 0; k < solutions[i][j]; k++) printf("- ");
            printf("* ");
            for (k = QUEENS - 1; k > solutions[i][j]; k--) printf("- ");
            printf("\n");
        }
        printf("\n");
    }
}                                                 /* print_solutions */


/*
 *��������is_safe
 *���ܣ���鵱ǰ�ʺ����ڷŵ�λ���Ƿ����ѰڷŵĻʺ��λ�����ͻ
 *���룺chessboard[]Ϊ��Żʺ����ڷŵ�λ�õ�����;
 *      rowΪ��ǰλ�õ��У�
 *      colΪ��ǰλ�õ��С�
 *���������0����ǰλ���г�ͻ�����ɰڷŻʺ�
        ����1����ǰλ��û�г�ͻ�����԰ڷŻʺ�
 */
bool is_safe(int chessboard[], int row, int col)
{
    int i;

    for (i = 0; i < row; i++)
    {
		/*��鵱ǰλ���Ƿ����i�лʺ��λ�����ͻ*/
        if (collides(i, chessboard[i], row, col))
            return false;
    }                                             /* for */
    return true;
}                                                 /* is_safe */


/*
 *��������place_queens
 *���ܣ�Ϊ��ǰ�еĻʺ�Ѱ���ʵ��İڷ�λ�ã�
 *      ����ʺ�ڷ�������¼���ڷŵ�λ�ã�
 *      �ݹ�İڷź���Ļʺ�
 *      �����лʺ�ڷ���Ϻ󣬼�¼���õĽ⡣
 *���룺chessboard[]Ϊ��Żʺ����ڷ�λ�õ����飻
        rowΪ��ǰ�ʺ���Ҫ�ڷŵ��кš�
 *�������
 */
void place_queens(int chessboard[], int row)
{
    int i, col;

    if (row >= QUEENS)                            /*���лʺ���ڷ����*/
    {
		/* �����ݹ鲢��¼��ǰ�� */
        for (i = 0; i < QUEENS; i++)
        {
            solutions[solution_count][i] = chessboard[i];
        }
        solution_count++;
    }
    else
    {
		/*���лʺ�û�аں�*/
        for (col = 0; col < QUEENS; col++)        /*�ڵ�ǰ��Ѱ���ʵ�λ�ðڷŻʺ�*/
        {
            if (is_safe(chessboard, row, col))    /*��ǰλ�ò���ͻ*/
            {
                chessboard[row] = col;            /* �ڵ�ǰλ�÷���һ���ʺ� */
                place_queens(chessboard, row + 1);/* �ݹ������һ���ʺ� */
            }                                     /* if */
        }                                         /* for */
    }                                             /* else */
}                                                 /* place_queens */


/*
 *��������sequential_eight_queens
 *���ܣ����е����˻ʺ����⣬�������ӡ����
 *���룺��
 *�������
 */
void sequential_eight_queens()
{
    int chessboard[QUEENS];

    solution_count = 0;
	/*��ʼ���˻ʺ�����*/
    place_queens(chessboard, 0);
	/*��ӡ������*/
    print_solutions(solution_count, solutions);
}                                                 /*sequential_eight_queens*/


/*
 *��������eight_queens_master
 *���ܣ����ɳ�ʼ�����̣������䷢�͸����е��ӽ��̣�
 *      ���ӽ����н��ܲ���¼�⣻
 *      �������ӽ��̶�����ֹ��û���µĳ�ʼ������ʱ����ӡ���еĽ⡣
 *���룺nodesΪ�������н�����
 *�������
 */
void eight_queens_master(int nodes)
{
    MPI_Status status;
    int active_slaves = nodes - 1;                // except the master itself
    int new_task = NEW_TASK;
    int terminate = TERMINATE;
    int reply;
    int child;
    int num_solutions;
    int seed;

    while (active_slaves)                         /*��δ�������ӽ���*/
    {
		/*���ӽ����н��ܷ����ź�*/
        MPI_Recv(&reply, 1, MPI_INT, MPI_ANY_SOURCE, REPLY_TAG, MPI_COMM_WORLD, &status);

        child = status.MPI_SOURCE;

        if (reply == ACCOMPLISHED)                /*�ӽ���������������*/
        {
			/* ���ӽ��̽��ղ���¼�� */
            MPI_Recv(&num_solutions, 1, MPI_INT, child, NUM_SOLUTIONS_TAG, MPI_COMM_WORLD, &status);

            if (num_solutions > 0)
            {
                MPI_Recv(solutions[solution_count],
                    QUEENS * num_solutions, MPI_INT,
                    child, SOLUTIONS_TAG, MPI_COMM_WORLD, &status);
                solution_count += num_solutions;
            }
        }

        seed = generate_seed();                   /*������ʼ����*/
        if (seed)                                 /* �����µĳ�ʼ���� */
        {
			/* ���ӽ��̷���һ��new_task�ź� */
            MPI_Send(&new_task, 1, MPI_INT, child, REQUEST_TAG, MPI_COMM_WORLD);
			/* ���ӽ��̷���һ���Ϸ��������� */
            MPI_Send(&seed, 1, MPI_INT, child, SEED_TAG, MPI_COMM_WORLD);
        }
        else                                      /* ��������н� */
        {
			/* ���ӽ��̷�����ֹ�ź� */
            MPI_Send(&terminate, 1, MPI_INT, child, REQUEST_TAG, MPI_COMM_WORLD);
            active_slaves--;
        }
    }                                             /* while */

	/*��ӡ���н�*/
    print_solutions(solution_count, solutions);
}                                                 /* eight_queens_master */


/*
 *��������eight_queens_slave
 *���ܣ��������̽����µĳ�ʼ�������̣�
 *      �ڳ�ʼ�������̻�����������кϷ��Ľ⣻
 *      ����õĽⷢ�͸������̡�
 *���룺my_rankΪ�ӽ����������������еĽ��̱�ʶ��
 *�������
 */
void eight_queens_slave(int my_rank)
{
    MPI_Status status;
    int ready = READY;
    int accomplished = ACCOMPLISHED;
    bool finished = false;
    int request;
    int seed;
    int num_solutions = 0;
    int chessboard[QUEENS];

	/*�������̷���ready�ź�*/
    MPI_Send(&ready, 1, MPI_INT, 0, REPLY_TAG, MPI_COMM_WORLD);

    while (! finished)                            /*�ӽ���δ��ֹ*/
    {
		/* �������̽�����Ϣ */
        MPI_Recv(&request, 1, MPI_INT, 0, REQUEST_TAG, MPI_COMM_WORLD, &status);

        if (request == NEW_TASK)
        {
			/* �������̽��ճ�ʼ���� */
            MPI_Recv(&seed, 1, MPI_INT, 0, SEED_TAG, MPI_COMM_WORLD, &status);

			/* �ڳ�ʼ���̻�������� */
            chessboard[0] = seed / QUEENS;
            chessboard[1] = seed % QUEENS;

            solution_count = 0;
            place_queens(chessboard, 2);

			/* ���ⷢ�͸������� */
            /*�������̷���accomplished�ź�*/
            MPI_Send(&accomplished, 1, MPI_INT, 0, REPLY_TAG, MPI_COMM_WORLD);
            MPI_Send(&solution_count, 1, MPI_INT, 0, NUM_SOLUTIONS_TAG, MPI_COMM_WORLD);
            if (solution_count > 0)
            {
                MPI_Send(*solutions,
                    QUEENS * solution_count,
                    MPI_INT, 0, SOLUTIONS_TAG, MPI_COMM_WORLD);
            }
        }
        else                                      /* request == TERMINATE */
        {
			/*������Ҫ���ӽ�����ֹ*/
            finished = true;
        }
    }                                             /* whlie */

}                                                 /* eight_queens_slave */


/******************** main ********************/

/*
 *��������main
 *���ܣ�����MPI���㣻
 *      ȷ���������ͽ��̱�ʶ����
 *      ���������̺ʹӽ��̳��������˻ʺ����⡣
 *���룺argcΪ�����в���������
 *      argvΪÿ�������в�����ɵ��ַ������顣
 *���������0���������������������������
 */
int main(int argc, char* argv[])
{
    int nodes, my_rank;

	/*��ʼ��*/
    MPI_Init(&argc, &argv);
	/*ȷ���������еĽ��̸���*/
    MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	/*ȷ���Լ��ڹ������еĽ��̱�ʶ��*/
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (nodes == 1)                               /*ֻ��һ������ʱ���ô����㷨���*/
    {
        sequential_eight_queens();
    }

	/*�������˻ʺ�����*/
    if (! my_rank)
    {
        eight_queens_master(nodes);
    }
    else
    {
        eight_queens_slave(my_rank);
    }

	/*����MPI����*/
    MPI_Finalize();

    return 0;
}                                                 /* main */
