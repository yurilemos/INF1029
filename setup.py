f = open("floats_256_5.0f.dat", "a")

i = 0
j = 0

while i < 1024:
    while j < 1024:
        f.write("1.0f ")
        j += 1
    f.write("\n")
    j = 0
    i += 1

f.close()

#open and read the file after the appending:
# f = open("demofile2.txt", "r")
# print(f.read())