
def calculatePi():
    denom, op, pi = 3, True, 4
    for i in range(1000000):
        frac = (4 / denom)
        pi = pi - frac if op else pi + frac
        op = not op
        denom += 2
    print(pi)


calculatePi()
