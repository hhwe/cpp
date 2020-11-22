print("hello world")

def f():
    print("han hai yan")

if __name__ == "__main__":
    for i in range(1,10):
        for j in range(1,i +1):
            print("%d*%d=%2d" % (i, j, i * j), end=" ")
            f()
        print()

i = 1
while i < 6:
    j = 0
    while j < i:
        print("* ", end=" ")
        j += 1
    print()
    i += 1