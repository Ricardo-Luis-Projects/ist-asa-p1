import random
import sys
import time
import matplotlib.pyplot as plt
import subprocess

def gen_input(n, min, max):
    """
    Generates a random list of integers.
    """
    return [random.randint(min, max) for _ in range(n)]

# Function to find the length of the longest increasing subsequence
def LIS(arr, i, n, prev):
 
    # Base case: nothing is remaining
    if i == n:
        return 0
 
    # case 1: exclude the current element and process the
    # remaining elements
    excl = LIS(arr, i + 1, n, prev)
 
    # case 2: include the current element if it is greater
    # than the previous element in LIS
    incl = 0
    if arr[i] > prev:
        incl = 1 + LIS(arr, i + 1, n, arr[i])
 
    # return the maximum of the above two choices
    return max(incl, excl)

def numOfIncSubseqOfSizeK(arr, n, k):
 
    dp = [[0 for i in range(n)]
             for i in range(k)]
              
    # count of increasing subsequences
    # of size 1 ending at each arr[i]
    for i in range(n):
        dp[0][i] = 1
 
    # building up the matrix dp[][]
    # Here 'l' signifies the size of
    # increasing subsequence of size (l+1).
    for l in range(1, k):
 
        # for each increasing subsequence of
        # size 'l' ending with element arr[i]
        for i in range(l, n):
 
            # count of increasing subsequences of
            # size 'l' ending with element arr[i]
            dp[l][i] = 0
            for j in range(l - 1, i):
                if (arr[j] < arr[i]):
                    dp[l][i] += dp[l - 1][j]
             
    # Sum up the count of increasing subsequences
    # of size 'k' ending at each element arr[i]
    Sum = 0
    for i in range(k - 1, n):
        Sum += dp[k - 1][i]
 
    # required number of increasing
    # subsequences of size k
    return Sum

def lis_length(input):
    """
    Counts the length of the longest strictly increasing subsequence.
    """
    return LIS(input, 0, len(input), -sys.maxsize)

def count_subseq(input, n):
    """
    Counts the number of strictly increasing subsequences of length n.
    """
    return numOfIncSubseqOfSizeK(input, len(input), n)

def brute_force(input):
    """
    Counts the length and count of longest strictly increasing subsequences.
    """
    l = lis_length(input)
    c = count_subseq(input, l)
    return l, c

def get_si_subsequences(input):
    """
    Returns all of the subsequences of input.
    """
    if len(input) == 0:
        return []
    elif len(input) == 1:
        return [input]
    else:
        ss1 = get_si_subsequences(input[1:])
        ss2 = list(filter(lambda x: x[0] > input[0], ss1))
        return ss1 + [[input[0]] + i for i in ([[]] + ss2)]

def brute_force_2(x, y):
    """
    Counts the length and count of longest strictly increasing common subsequences.
    """
    # Get the all of the subsequences of x
    x = get_si_subsequences(x)
    y = get_si_subsequences(y)
    common = []
    for i in x:
        if i in common:
            common.append(i)
        else:
            for j in y:
                if i == j:
                    common.append(j)
            if i in common:
                common.append(i)

    if len(common) == 0:
        return 0, 0

    # Get only the longest common subsequences
    common.sort(key=len, reverse=True)
    l = len(common[0])
    c = 0
    for seq in common:
        if len(seq) == l:
            c += 1
        else:
            break
    return l, c

def solve(input):
    """
    Calls the C++ executable and returns the output.
    """
    import subprocess
    p = subprocess.Popen("./file", stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    p.stdin.write(bytes("1\n" + " ".join([str(i) for i in input]) + "\n", "utf-8"))
    p.stdin.close()
    return tuple(int(i) for i in p.stdout.read().decode("utf-8").split(" "))
    
def solve_2(x, y):
    """
    Calls the C++ executable and returns the output.
    """
    import subprocess
    p = subprocess.Popen("./file", stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    p.stdin.write(bytes("2\n" + " ".join([str(i) for i in x]) + "\n" + " ".join([str(i) for i in y]) + "\n", "utf-8"))
    p.stdin.close()
    return tuple(int(i) for i in p.stdout.read().decode("utf-8").split(" "))

def test():
    for i in range(1, 1000):
        input = gen_input(50, 0, 5)
        l, c = brute_force(input)
        l_, c_ = solve(input)
        if l != l_ or c != c_:
            print("FAILED:", input, l, c, l_, c_)
            break
        else:
            print("OK")

def test2():
    n = 20
    for i in range(1, 1000):
        x = gen_input(n, 0, 50)
        y = gen_input(n, 0, 50)
        l, c = brute_force_2(x, y)
        l_, = solve_2(x, y)
        if l != l_:
            print("FAILED:", x, y, l, l_)
            break
        else:
            print("OK")
        
def stress(n, m):
    """
    Generate n random inputs of size m and solves them, returning the time taken.
    """
    p = subprocess.Popen("./file", stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    p.stdin.write(bytes(str(m) + " " + str(n) + "\n", "utf-8"))
    p.stdin.close()
    return float(p.stdout.read().decode("utf-8"))

def main():
    """
    Main function.
    """
    fig, ax = plt.subplots()
    ax.set(xlabel='n', ylabel='time (ms)', title='Stress test')
    ax.grid()

    xs = []
    ys = []
    for n in range(5, 10000, 100):
        # Stress test
        for i in range(1, 100):
            xs.append(n)
            ys.append(stress(1, n))
        print("{}".format(n))

    ax.scatter(xs, ys)
    plt.show()
