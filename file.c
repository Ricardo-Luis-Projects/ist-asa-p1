#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define BENCHMARK

/** Describes a solution to the problem. */
struct solution
{
    long long a; /**< The length of the longest strictly increasing subsequence. */
    long long b; /**< The number of longest strictly increasing subsequences. */
};

/** Combines two partial solutions to the problem.
 *
 * @param lhs The first partial solution.
 * @param rhs The second partial solution.
 * @return The combined partial solution.
 */
struct solution combine_solutions(struct solution lhs, struct solution rhs)
{
    if (lhs.a == rhs.a)
        return (struct solution){lhs.a, lhs.b + rhs.b};
    else if (lhs.a > rhs.a)
        return lhs;
    else
        return rhs;
}

/** Data structure used to update and query the partial solutions to the problem. */
struct fenwick_tree
{
    size_t size;           /**< The size of the fenwick tree. */
    struct solution* data; /**< The array of nodes in the fenwick tree. */
};

/** Initializes a fenwick tree with the given size.
 *
 * @param tree The fenwick tree to initialize.
 * @param leaves The size of the fenwick tree.
 * @return The initialized fenwick tree.
 */
void fenwick_tree_init(struct fenwick_tree* tree, size_t size)
{
    /* Round up the size to the next power of 2 + 1. */
    tree->size = 1 + (1 << (size_t)ceil(log((double)size - 1.0) / log(2.0)));
    tree->data = (struct solution*)malloc(sizeof(struct solution) * (tree->size * 2 - 1));
    memset(tree->data, 0, sizeof(struct solution) * (tree->size * 2 - 1));
}

/** Frees the memory allocated for the fenwick tree.
 *
 * @param tree The fenwick tree to free.
 */
void fenwick_tree_free(struct fenwick_tree* tree)
{
    free(tree->data);
}

/** Updates the solution at the given index in the fenwick tree.
 *
 * @param tree The fenwick tree to update.
 * @param index The index to update.
 * @param solution The solution to update with.
 */
void fenwick_tree_update(struct fenwick_tree* tree, size_t index, struct solution solution)
{
    if (index == 0)
        tree->data[0] = combine_solutions(tree->data[0], solution);
    else
        for (; index < tree->size; index += index & -index)
            tree->data[index] = combine_solutions(tree->data[index], solution);
}

/** Queries the solution at the given prefix of elements in the fenwick tree.
 *
 * @param tree The fenwick tree to query.
 * @param r The right bound of the query.
 * @return The solution to the query.
 */
struct solution fenwick_tree_query(struct fenwick_tree* tree, size_t r)
{
    struct solution ans = tree->data[0];
    for (; r != 0; r -= r & -r)
        ans = combine_solutions(ans, tree->data[r]);
    return ans;
}

/** Sorts the indices of the given array in ascending order.
 *
 * @param array The array of integers to sort.
 * @param in_i The array of indices to sort.
 * @param out_i The sorted array of indices.
 * @param size The number of elements in the array.
 */
void sort_indices(long long* array, size_t* in_i, size_t* out_i, size_t size)
{
    size_t i = 0, j = 0;
    size_t size_i = size / 2, size_j = size - size / 2;

    if (size == 1)
    {
        out_i[0] = in_i[0];
        return;
    }

    /** Sort the two halves of the array. */
    sort_indices(array, in_i, out_i, size_i);
    sort_indices(array, in_i + size_i, out_i + size_i, size_j);
    memcpy(in_i, out_i, sizeof(size_t) * size);

    /** Merge the two sorted halves of the array. */
    while (i + j < size)
    {
        if (j >= size_j || (i < size_i && array[in_i[i]] < array[in_i[size_i + j]]))
        {
            out_i[i + j] = in_i[i];
            i += 1;
        }
        else
        {
            out_i[i + j] = in_i[size_i + j];
            j += 1;
        }
    }
}

/** Normalizes two contiguous arrays of numbers so that every number is between 1 and n + m, and the order is preserved,
 * where n is the size of the  first array and m is the size of the second array. Only the common elements are kept. If
 * m is NULL, then all elements of the first array are kept.
 *
 * @param array The arrays to normalize.
 * @param n The size of the first array.
 * @param m The size of the second array.
 * @return The maximum value in the arrays.
 */
long long normalize(long long* array, size_t* n, size_t* m)
{
    size_t i, j, sz = m ? *n + *m : *n, prev_n;
    long long* tmp;
    size_t* indices = (size_t*)malloc(sizeof(size_t) * sz * 2);
    long long max_value = 0, found, prev;

    for (i = 0; i < sz; i++)
        indices[sz + i] = i;
    sort_indices(array, indices + sz, indices, sz);

    /* Remove non-common elements. */
    for (i = 0; i < sz - 1 && m; i++)
    {
        if (array[indices[i]] != array[indices[i + 1]])
        {
            indices[i] = -1;
            if (i == sz - 2)
                indices[i + 1] = -1;
        }
        else
        {
            /* Search for another element with the same value in the opposite array. */
            found = 0;
            for (j = i + 1; j < sz && array[indices[j]] == array[indices[i]]; j++)
                if ((indices[i] < *n && indices[j] >= *n) || (indices[i] >= *n && indices[j] < *n))
                    found = 1;

            /* If there is no such element, then remove all elements with the same value. */
            if (!found)
                for (; i < j; i++)
                    indices[i] = -1;
            else
                i = j;
            i -= 1;
        }
    }

    /* Temporary array to store the normalized array, in case some elements will be removed. */
    if (m)
    {
        tmp = (long long*)malloc(sizeof(long long) * sz);
        memset(tmp, 0, sizeof(long long) * sz);
    }

    /* Assign the normalized values. */
    for (i = 0, found = 0; i < sz; i++)
    {
        if (indices[i] == -1)
            continue;
        if (!found || array[indices[i]] != prev)
            max_value += 1;
        found = 1;
        prev = array[indices[i]];
        if (m)
            tmp[indices[i]] = max_value;
        else
            array[indices[i]] = max_value;
    }

    /* Move back the elements to fill the gaps in the array. */
    if (m)
    {
        prev_n = *n;
        for (i = 0, j = 0; i < sz; i++)
        {
            if (tmp[i] == 0)
                i < prev_n ? (*n)-- : (*m)--;
            else
                array[j++] = tmp[i];
        }

        free(tmp);
    }

    free(indices);

    return max_value;
}

/** Finds the solution to the first problem.
 *
 * @param array The input array.
 * @param n The size of the array.
 * @return The solution to the problem.
 */
struct solution first_problem(long long* array, size_t n)
{
    struct solution query;
    struct fenwick_tree tree;
    size_t max_value = normalize(array, &n, NULL);
    size_t i;

    /* Initialize the fenwick tree and set the base case. */
    fenwick_tree_init(&tree, max_value + 1);
    fenwick_tree_update(&tree, 0, (struct solution){0, 1});

    /* Compute the partial solutions. */
    for (i = 0; i < n; i++)
    {
        query = fenwick_tree_query(&tree, array[i] - 1);
        fenwick_tree_update(&tree, array[i], (struct solution){query.a + 1, query.b});
    }

    /* Get the complete solution. */
    query = fenwick_tree_query(&tree, max_value);
    fenwick_tree_free(&tree);

    return query;
}

/** Finds the solution to the second problem.
 *
 * @param array The input arrays, in order.
 * @param n The size of the first array.
 * @param m The size of the second array.
 * @return The solution to the problem.
 */
long long second_problem(long long* array, size_t n, size_t m)
{
    size_t max_value = normalize(array, &n, &m);
    size_t i, j, ki, kn, next_k;
    size_t* ks;
    struct solution query;
    struct fenwick_tree* trees;

    if (n == 0 || m == 0)
        return 0;

    /* Allocate the memory needed. */
    ks = (size_t*)malloc(sizeof(size_t) * m);
    trees = (struct fenwick_tree*)malloc(sizeof(struct fenwick_tree) * m);

    /* Initialize the fenwick trees. */
    for (i = 0; i < m; i++)
    {
        fenwick_tree_init(&trees[i], max_value + 1);
        fenwick_tree_update(&trees[i], 0, (struct solution){0, 1});
    }

    /* Compute the partial solutions. */
    for (i = 0; i < n; i++)
    {
        /* Find the ks where A[i] = Y[k]. */
        for (j = 0, kn = 0; j < m; j++)
            if (array[i] == array[n + j])
                ks[kn++] = j;

        for (ki = 0; ki < kn; ki++)
        {
            /* The limit of the following loop is the next element of the ks array (or the end of the Y sequence). */
            next_k = ki < kn - 1 ? ks[ki + 1] : m;

            /* Find the maximum A value and the sum of every B with a maximum A value up to the current element. */
            query = fenwick_tree_query(&trees[ks[ki]], array[i] - 1);

            /* Update the fenwick trees with the new maximum A value and the sum of every B with a maximum A value. */
            for (j = ks[ki]; j < next_k; j++)
                fenwick_tree_update(&trees[j], array[i], (struct solution){query.a + 1, query.b});
        }
    }

    /* Get the complete solution. */
    query = fenwick_tree_query(&trees[m - 1], max_value);

    /* Free the memory. */
    for (i = 0; i < m; i++)
        fenwick_tree_free(&trees[i]);
    free(trees);
    free(ks);

    return query.a;
}

/** Represents an expandable array of long longs. */
struct vector
{
    long long* array; /** The array. */
    size_t size;      /** The size of the array. */
    size_t capacity;  /** The capacity of the array. */
};

/** Initializes an expandable array of long longs.
 *
 * @param vector The vector to initialize.
 * @param capacity The initial capacity of the vector.
 */
void vector_init(struct vector* vector, size_t capacity)
{
    vector->array = (long long*)malloc(sizeof(long long) * capacity);
    vector->size = 0;
    vector->capacity = capacity;
}

/** Frees the memory allocated by an expandable array of long longs.
 *
 * @param vector The vector to free.
 */
void vector_free(struct vector* vector)
{
    free(vector->array);
}

/** Pushes a long long onto the end of an expandable array of long longs.
 *
 * @param vector The vector to push onto.
 * @param value The value to push.
 */
void vector_push(struct vector* vector, long long value)
{
    if (vector->size == vector->capacity)
    {
        vector->capacity *= 2;
        vector->array = (long long*)realloc(vector->array, sizeof(long long) * vector->capacity);
    }

    vector->array[vector->size++] = value;
}

/** Pushes numbers read from stdin to a vector, until a newline is encountered.
 *
 * @param vector The vector to push onte.
 * @return The number of numbers read.
 */
size_t read_vector(struct vector* vector)
{
#ifdef BENCHMARK
    size_t count, i;

    if (!scanf("%ld", &count))
        return 0;
    srand(time(NULL));
    count = rand() % count + 1;

    for (i = 0; i < count; i++)
        vector_push(vector, rand() % (count * 2));

#else
    size_t count = 1;
    char c;

    vector_push(vector, 0);
    for (;;)
    {
        c = getchar();
        if (c == '\n' || c == EOF)
            break;

        if (c == ' ')
        {
            vector_push(vector, 0);
            count += 1;
        }
        else
            vector->array[vector->size - 1] = vector->array[vector->size - 1] * 10 + c - '0';
    }
#endif

    return count;
}

int main(int argc, char** argv)
{
    struct vector vector;
    size_t p, n, m;
#ifdef BENCHMARK
    clock_t begin;
#endif

    /* Read the input. */
    if (!scanf("%ld\n", &p))
        return 1;

    vector_init(&vector, 16);
    if (p == 1)
    {
        struct solution solution;
        n = read_vector(&vector);
#ifdef BENCHMARK
        begin = clock();
#endif
        solution = first_problem(vector.array, n);
#ifdef BENCHMARK
        printf("%f %ld ", (double)(clock() - begin) / CLOCKS_PER_SEC, n);
#endif
        printf("%lld %lld\n", solution.a, solution.b);
    }
    else if (p == 2)
    {
        long long solution;
        n = read_vector(&vector);
        m = read_vector(&vector);
#ifdef BENCHMARK
        begin = clock();
#endif
        solution = second_problem(vector.array, n, m);
#ifdef BENCHMARK
        printf("%f %ld ", (double)(clock() - begin) / CLOCKS_PER_SEC, n + m);
#endif
        printf("%lld\n", solution);
    }

    vector_free(&vector);

    return 0;
}
