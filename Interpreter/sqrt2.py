

i, squared = 1.0, 1.0
while (.00001 < (2 - squared)):
    i += .000001;
    squared = i * i
print(i)
print(squared)
